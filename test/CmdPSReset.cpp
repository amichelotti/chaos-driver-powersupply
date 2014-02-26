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

#define LOG_HEAD_CmdPSReset LOG_TAIL(CmdPSReset)

#define CMDCU_ LAPP_ << LOG_HEAD_CmdPSReset
#define CMDCUDBG_ LDBG_ << LOG_HEAD_CmdPSReset

namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;



// return the implemented handler
uint8_t own::CmdPSReset::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSReset::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	/*
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
				TROW_ERROR(1, boost::str( boost::format("Bad state for reset comamnd %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
			}
	}
	*/
	
	//set comamnd timeout for this instance
	CMDCUDBG_ << "Checking for timout";
	if(*i_command_timeout) {
		CMDCUDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	//send comamnd to driver
	CMDCUDBG_ << "Resetting alarms";
	if(powersupply_drv->resetAlarms(0) != 0) {
		TROW_ERROR(2, boost::str( boost::format("Error resetting the allarms in state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
	}
	
	/*	CMDCUDBG_ << "Go to standby";
	if(powersupply_drv->standby() != 0) {
		TROW_ERROR(3, boost::str( boost::format("Error set to standby in state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
	}
	*/
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
		BC_END_RUNNIG_PROPERTY
		return;
	}
	
	if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ALARM ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ERROR ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_UKN ) {
		setWorkState(false);
		TROW_ERROR(1, boost::str( boost::format("Bad state got = %1% - [%2%]") % o_status_id % o_status), std::string(__FUNCTION__))
	}
}

bool own::CmdPSReset::timeoutHandler() {
	setWorkState(false);
	TROW_ERROR(1, "Comamdn operation has gone on timeout", std::string(__FUNCTION__))
	return true;
}
