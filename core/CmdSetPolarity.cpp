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

void own::CmdSetPolarity::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
        AbstractPowerSupplyCommand::acquireHandler();

	int err = 0;
        setAlarmSeverity("polarity_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);
        setAlarmSeverity("polarity_value_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelClear);
        uint64_t timeo;
    if(!data || !data->hasKey(CMD_PS_SET_POLARITY_VALUE) ) {
		SCLERR_ << "Type of polarity not passed";
                setAlarmSeverity("polarity_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

		BC_FAULT_RUNNING_PROPERTY;
		return;
    }
        if(powersupply_drv->getFeatures()& common::powersupply::POWER_SUPPLY_FEAT_BIPOLAR){
            	//SCLERR_ << "invalid command for bipolars";
                setAlarmSeverity("polarity_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                           metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,"invalid command for bipolars");

                BC_FAULT_RUNNING_PROPERTY;
		return;
        }

	SCLDBG_ << "Checking for timout";
	if(*p_setTimeout) {
		SCLDBG_ << "Timeout will be set to ms -> " << *p_setTimeout;
                timeo =  *p_setTimeout;
	} else {
		//set five second of timeout
		SCLDBG_ << "Timeout will be set to ms ->" << DEFAULT_COMMAND_TIMEOUT_MS;
                timeo = DEFAULT_COMMAND_TIMEOUT_MS;
	}
        setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)timeo);

	int32_t polarity_readout = 0;
    polarity_set_point = data->getInt32Value(CMD_PS_SET_POLARITY_VALUE);
    SCLAPP_ << "Set polarity called with value " << polarity_set_point;
    
    if(*c_stbyOnPol){
        if(*o_stby==false){
          // SCLERR_ << "## cannot change polarity since is not in standby";
           setAlarmSeverity("polarity_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
           metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError, "cannot change polarity since is not in standby" );

            BC_FAULT_RUNNING_PROPERTY;
	    return;

        }
    }
     
    
    if((err = powersupply_drv->setPolarity(polarity_set_point)) != 0) {
           SCLERR_ << "## error setting polarity to:"<<polarity_set_point;
           setAlarmSeverity("polarity_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

           BC_FAULT_RUNNING_PROPERTY;
	   return;

    }

	//set the operation flag on
	setWorkState(true);
        *i_pol=polarity_set_point;
        getAttributeCache()->setInputDomainAsChanged();
//        pushInputDataset();
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,boost::str( boost::format("performing command set polarity :%1% timeout %2% ms") % polarity_set_point %timeo ) );

    //run in exclusive mode
    BC_NORMAL_RUNNING_PROPERTY
}

//custom acquire method
void own::CmdSetPolarity::acquireHandler() {
    AbstractPowerSupplyCommand::acquireHandler();
    getAttributeCache()->setOutputDomainAsChanged();
}

//Correlation and commit phase
void own::CmdSetPolarity::ccHandler() {
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	if(polarity_set_point == *o_pol){
		//set the operation flag on
		SCLDBG_ << boost::str(boost::format("[metric] We have reached the polarity in %1% milliseconds") % elapsed_msec);
		BC_END_RUNNING_PROPERTY
	}
}

//manage the timeout
bool own::CmdSetPolarity::timeoutHandler() {
	SCLDBG_ << "timeoutHandler";
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	if(polarity_set_point == *o_pol){
		//set the operation flag on
		SCLDBG_ << boost::str(boost::format("[metric] Timeout reached in with set-point %1% and readout %2% in %3% milliseconds") % polarity_set_point % *o_pol % elapsed_msec);
                BC_END_RUNNING_PROPERTY;
                return false;
	}else{
               setAlarmSeverity("polarity_value_not_reached",chaos::common::alarm::MultiSeverityAlarmLevelWarning);

		SCLERR_ << boost::str(boost::format("[metric] Timeout reached in WITHOUT set-point %1% and readout %2% in %3% milliseconds") % polarity_set_point % *o_pol % elapsed_msec);

	}
        BC_END_RUNNING_PROPERTY

	return false;
}