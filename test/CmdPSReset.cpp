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

#define CMDCU_ LAPP_ << "[CmdPSReset] - "
#define CMDCUDBG_ LDBG_ << "[CmdPSReset] - "

namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;


// return the implemented handler
uint8_t own::CmdPSReset::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSReset::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);

	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
			case common::powersupply::POWER_SUPPLY_STATE_ERROR:
			case common::powersupply::POWER_SUPPLY_STATE_UKN:
			//i need to be in operational to exec
			CMDCUDBG_ << "We can start the reset command";
			break;
			
		default:
			if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_OPEN)||
			   (*o_status_id != common::powersupply::POWER_SUPPLY_STATE_ON)) {
				throw chaos::CException(1, boost::str( boost::format("Bas state for reset comamnd %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
			}
	}
	
	
	//set comamnd timeout for this instance
	CMDCUDBG_ << "Checking for timout";
	if(*i_command_timeout) {
		CMDCUDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
		setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	//send comamnd to driver
	CMDCUDBG_ << "Resetting allarm";
	if(powersupply_drv->resetAlarms(0) != 0) {
		throw chaos::CException(2, boost::str( boost::format("Error resetting the allarms in state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
	}
	
	CMDCUDBG_ << "Go to standby";
	if(powersupply_drv->standby() != 0) {
		throw chaos::CException(2, boost::str( boost::format("Error set to standby in state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
	}
	
	//set working flag
	setWorkState(true);
}

void own::CmdPSReset::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
	
	CMDCUDBG_ << "Reset command correlation";
	if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
		CMDCUDBG_ << "We have reached standby state";
		setWorkState(false);
		//we are terminated the command
		SL_END_RUNNIG_STATE
		return;
	}
	
	if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ALARM ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ERROR ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_UKN ) {
		std::string error =  boost::str( boost::format("Bad state got = %1% - [%2%]") % o_status_id % o_status);
		writeErrorMessage(error);
		setWorkState(false);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}
}

bool own::CmdPSReset::timeoutHandler() {
	//move the state machine on fault
	std::string error =  "Comamdn operation has gone on timeout";
	writeErrorMessage(error);
	setWorkState(false);
	throw chaos::CException(1, error.c_str(), __FUNCTION__);
	return true;
}