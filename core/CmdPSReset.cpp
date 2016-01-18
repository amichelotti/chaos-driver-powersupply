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

#define CMDCU_ INFO_LOG(CmdPSReset)
#define CMDCUDBG_ DBG_LOG(CmdPSReset)
#define CMDCUERR_ ERR_LOG(CmdPSReset)

namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSReset,CMD_PS_RESET_ALIAS,
                                                          "Reset alarms",
                                                          "2d4c97a2-35da-11e5-b01c-57447c7fdd89")
BATCH_COMMAND_CLOSE_DESCRIPTION()

// return the implemented handler
uint8_t own::CmdPSReset::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSReset::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	i_command_timeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "command_timeout");
	
	//set comamnd timeout for this instance
	CMDCUDBG_ << "Checking for timout";
	if(*i_command_timeout) {
		CMDCUDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	} else {
		//set five second of timeout
		CMDCUDBG_ << "Timeout will be set to ms -> 5000";
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)5000);
	}
	
	//send comamnd to driver
	CMDCUDBG_ << "Resetting alarms";
	if(powersupply_drv->resetAlarms(0) != 0) {
		LOG_AND_TROW(CMDCUERR_, 1, boost::str( boost::format("Error resetting the allarms in state %1%[%2%]") % o_status % *o_status_id));
	}

	//set working flag
	setWorkState(true);
	BC_EXEC_RUNNIG_PROPERTY
}

void own::CmdPSReset::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	if(*o_alarms == 0) {
		CMDCUDBG_ << "We have reset the alarms";
		CMDCUDBG_ << boost::str(boost::format("[metric] We have reset the alarms in %1% milliseconds") % elapsed_msec);
		setWorkState(false);
		//we are terminated the command
		BC_END_RUNNIG_PROPERTY
	}
}

bool own::CmdPSReset::timeoutHandler() {
	setWorkState(false);
	CMDCUERR_ << ("We have reached timout on reset alarms");
	return false;
}
