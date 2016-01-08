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

#define LOG_HEAD_CmdPSSetCurrent LOG_TAIL(CmdSetPolarity)
#define SCLAPP_ LAPP_ << LOG_HEAD_CmdPSSetCurrent
#define SCLDBG_ LDBG_ << LOG_HEAD_CmdPSSetCurrent
#define SCLERR_ LERR_ << LOG_HEAD_CmdPSSetCurrent


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

    o_polarity = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");
	i_command_timeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "command_timeout");
	
    if(!data || !data->hasKey(CMD_PS_SET_POLARITY_VALUE) ) {
		SCLERR_ << "Type of polarity not passed";
		BC_END_RUNNIG_PROPERTY;
		return;
    }
	int32_t polarity_readout = 0;
    int32_t polarity = data->getInt32Value(CMD_PS_SET_POLARITY_VALUE);
    SCLAPP_ << "Set polarity called with value " << polarity;
    
	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
		case common::powersupply::POWER_SUPPLY_STATE_ERROR:
		case common::powersupply::POWER_SUPPLY_STATE_UKN:
        case common::powersupply::POWER_SUPPLY_STATE_OPEN:
		case common::powersupply::POWER_SUPPLY_STATE_ON:
                //i need to be in operational to exec
			SCLERR_ << boost::str( boost::format("Bad state for set polarity comamnd %1%[%2%]") % o_status % *o_status_id);
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

	if((err = powersupply_drv->setPolarity(polarity)) != 0) {
		LOG_AND_TROW(SCLERR_, 1, boost::str( boost::format("Error setting the polarity on driver with code %1%") % err));
	}

    //read the polarity
    if((err = powersupply_drv->getPolarity(&polarity_readout)) != 0){
        LOG_AND_TROW(SCLERR_, 2, boost::str( boost::format("Error getting the polarity from driver with code %1%") % err));
	}
    *o_polarity = polarity_readout;
    BC_END_RUNNIG_PROPERTY
}
