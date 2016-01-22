/*
 *	CmdSetPolarity.cpp
 *	!CHAOS
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "CmdSetPolarity.h"


#include <boost/format.hpp>

#define SCLAPP_ INFO_LOG(CmdSetPolarity) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdSetPolarity) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdSetPolarity) << "[" << getDeviceID() << "] "

namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdSetPolarity,CMD_PS_SET_POLARITY_ALIAS,
                                                          "Set the polarity ",
                                                          "adf2973c-35dd-11e5-855a-734defc0b6db")
BATCH_COMMAND_ADD_INT32_PARAM(CMD_PS_SET_POLARITY_VALUE, "Set the polarity >0 to switch positive, <0 to switch negative, 0 open", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)

BATCH_COMMAND_CLOSE_DESCRIPTION()

    // return the implemented handler
uint8_t own::CmdSetPolarity::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdSetPolarity::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	int err = 0;

    if(!data || !data->hasKey(CMD_PS_SET_POLARITY_VALUE) ) {
		SCLERR_ << "Type of polarity not passed";
		BC_END_RUNNIG_PROPERTY;
		return;
    }

	o_polarity = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");
	i_command_timeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "command_timeout");

	SCLDBG_ << "Checking for timout";
	if(*i_command_timeout) {
		SCLDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	} else {
		//set five second of timeout
		SCLDBG_ << "Timeout will be set to ms -> 5000";
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)5000);
	}
	int32_t polarity_readout = 0;
    polarity_set_point = data->getInt32Value(CMD_PS_SET_POLARITY_VALUE);
    SCLAPP_ << "Set polarity called with value " << polarity_set_point;
    
	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
		case common::powersupply::POWER_SUPPLY_STATE_ERROR:
		case common::powersupply::POWER_SUPPLY_STATE_UKN:
        case common::powersupply::POWER_SUPPLY_STATE_OPEN:
		case common::powersupply::POWER_SUPPLY_STATE_ON:
                //i need to be in operational to exec
			SCLERR_ << boost::str( boost::format("Bad state for set polarity command %1%[%2%]") % o_status % *o_status_id);
			BC_END_RUNNIG_PROPERTY;
			return;
			

		case common::powersupply::POWER_SUPPLY_STATE_STANDBY:
			SCLDBG_ << "We can start the set polarity command";
			break;
			
		default:
			SCLERR_ << boost::str(boost::format("Unrecognized state %1%[%2%]") % o_status % *o_status_id);
			BC_END_RUNNIG_PROPERTY;
			return;
	}

	if((err = powersupply_drv->setPolarity(polarity_set_point)) != 0) {
		LOG_AND_TROW(SCLERR_, 1, boost::str( boost::format("Error setting the polarity on driver with code %1%") % err));
	}

	//set the operation flag on
	setWorkState(true);

    //run in exclusive mode
    BC_EXEC_RUNNIG_PROPERTY
}

//custom acquire method
void own::CmdSetPolarity::acquireHandler() {
	int err = 0;
	if((err = powersupply_drv->getPolarity(o_polarity)) != 0){
	    LOG_AND_TROW(SCLERR_, 2, boost::str( boost::format("Error getting the polarity from driver with code %1%") % err));
	}
}

//Correlation and commit phase
void own::CmdSetPolarity::ccHandler() {
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	if(polarity_set_point == *o_polarity){
		//set the operation flag on
		setWorkState(false);
		SCLDBG_ << boost::str(boost::format("[metric] We have reached the polarity in %1% milliseconds") % elapsed_msec);
		BC_END_RUNNIG_PROPERTY
	}
}

//manage the timeout
bool own::CmdSetPolarity::timeoutHandler() {
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	setWorkState(false);
	if(polarity_set_point == *o_polarity){
		//set the operation flag on
		SCLDBG_ << boost::str(boost::format("[metric] Timeout reached in with set-point %1% and readout %2% in %3% milliseconds") % polarity_set_point % *o_polarity % elapsed_msec);
		BC_END_RUNNIG_PROPERTY
	}else{
		SCLERR_ << boost::str(boost::format("[metric] Timeout reached in WITHOUT set-point %1% and readout %2% in %3% milliseconds") % polarity_set_point % *o_polarity % elapsed_msec);
		BC_FAULT_RUNNIG_PROPERTY
	}
	return false;
}