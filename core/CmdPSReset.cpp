/*
 *	CmdPSReset.cpp
 *	!CHOAS
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


#include "CmdPSReset.h"

#include <boost/format.hpp>

#define CMDCU_ INFO_LOG(CmdPSReset) << "[" << getDeviceID() << "] "
#define CMDCUDBG_ DBG_LOG(CmdPSReset) << "[" << getDeviceID() << "] "
#define CMDCUERR_ ERR_LOG(CmdPSReset) << "[" << getDeviceID() << "] "

namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSReset,CMD_PS_RESET_ALIAS,
                                                          "Reset alarms",
                                                          "2d4c97a2-35da-11e5-b01c-57447c7fdd89")
BATCH_COMMAND_CLOSE_DESCRIPTION()
using namespace chaos::cu::control_manager;

// return the implemented handler
uint8_t own::CmdPSReset::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler() |chaos_batch::HandlerType::HT_Acquisition|chaos_batch::HandlerType::HT_Correlation;;
}

own::CmdPSReset::~CmdPSReset(){

}
void own::CmdPSReset::setHandler(c_data::CDataWrapper *data) {
	setWorkState(true);

	AbstractPowerSupplyCommand::setHandler(data);
    AbstractPowerSupplyCommand::acquireHandler();

	//set comamnd timeout for this instance
	CMDCUDBG_ << "Checking for timout";
	if(*p_setTimeout) {
		CMDCUDBG_ << "Timeout will be set to ms -> " << *p_setTimeout;
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *p_setTimeout);
	} else {
		//set five second of timeout
		CMDCUDBG_ << "Timeout will be set to ms -> "<<DEFAULT_COMMAND_TIMEOUT_MS;
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)DEFAULT_COMMAND_TIMEOUT_MS*1000);
	}
	
	//send comamnd to driver
	CMDCUDBG_ << "Resetting alarms";
	if(powersupply_drv->resetAlarms(0) != 0) {
		CMDCUERR_<<"## cannot reset alarms";
                BC_FAULT_RUNNING_PROPERTY;
                return;
        }
    setStateVariableSeverity(StateVariableTypeAlarmDEV, chaos::common::alarm::MultiSeverityAlarmLevelClear);
    setStateVariableSeverity(StateVariableTypeAlarmCU, chaos::common::alarm::MultiSeverityAlarmLevelClear);

	//set working flag
	BC_NORMAL_RUNNING_PROPERTY;
	getAttributeCache()->setOutputDomainAsChanged();
}

void own::CmdPSReset::ccHandler() {
        AbstractPowerSupplyCommand::acquireHandler();
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	if(*o_alarms == 0) {
		CMDCUDBG_ << boost::str(boost::format("[metric] We have reset the alarms in %1% milliseconds") % elapsed_msec);
		//we are terminated the command

		BC_END_RUNNING_PROPERTY;
	}
	getAttributeCache()->setOutputDomainAsChanged();
}

bool own::CmdPSReset::timeoutHandler() {
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	CMDCUERR_ << ("We have reached timout on reset alarms");
	if(*o_alarms == 0) {
		CMDCUDBG_ << boost::str(boost::format("[metric] We have reset the alarms on timeout in %1% milliseconds") % elapsed_msec);
		//we are terminated the command
		BC_END_RUNNING_PROPERTY;
	} else {
		CMDCUERR_ << boost::str(boost::format("[metric] We DON'T HAVE reset the alarms on timeout in %1% milliseconds") % elapsed_msec);
		BC_END_RUNNING_PROPERTY;
                
	}

	return false;
}
