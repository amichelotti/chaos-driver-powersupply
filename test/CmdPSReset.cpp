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

namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;


// return the implemented handler
uint8_t own::CmdPSReset::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler() |
	ccc_slow_command::HandlerType::HT_Correlation;
}

void own::CmdPSReset::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	int state;
	string state_str;

	getState(state, state_str);
	switch (state) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
			case common::powersupply::POWER_SUPPLY_STATE_ERROR:
			case common::powersupply::POWER_SUPPLY_STATE_UKN:
			//i need to be in operational to exec
			CMDCU_ << "Reset command callend on the rigth state";
			break;
			
		default:
			if((state != common::powersupply::POWER_SUPPLY_STATE_OPEN)||
			   (state != common::powersupply::POWER_SUPPLY_STATE_ON)) {
				throw chaos::CException(2, boost::str( boost::format("Bas state for reset  comamnd %1%[%2%]") % state_str % state), std::string(__FUNCTION__));
			}
	}
	
	
	//set comamnd timeout for this instance
	setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	
	//send comamnd to driver
	powersupply_drv->poweron();
}

void own::CmdPSReset::ccHandler() {
	int state_id;
	std::string state_str;
	getState(state_id, state_str);
	
	if(state_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
		//we are terminated the command
		SL_END_RUNNIG_STATE
		return;
	}
	
	if(state_id != common::powersupply::POWER_SUPPLY_STATE_ON) {
		std::string error =  boost::str( boost::format("Bad state got = %1% - [%2%]") % state_id % state_str);
		writeErrorMessage(error);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}
}

bool own::CmdPSReset::timeoutHandler() {
	//move the state machine on fault
	std::string error =  "Comamdn operation has gone on timeout";
	writeErrorMessage(error);
	throw chaos::CException(1, error.c_str(), __FUNCTION__);
	return true;
}