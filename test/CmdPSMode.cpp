//
//  CmdPSMode.cpp
//  PowerSupply
//
//  Created by Claudio Bisegni on 06/11/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdPSMode.h"

#include <boost/format.hpp>

#define CMDCU_ LAPP_ << "[CmdPSMode] - "

namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;

// return the implemented handler
uint8_t own::CmdPSMode::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler() |
	ccc_slow_command::HandlerType::HT_Correlation;
}

void own::CmdPSMode::setHandler(c_data::CDataWrapper *data) {
	CMDCU_ << "Mode Command set handler";
	
	AbstractPowerSupplyCommand::setHandler(data);
	int state;
	string state_str;
	//requested mode
	if(!data->hasKey(CMD_PS_MODE_TYPE)) {
		throw chaos::CException(0, "Mode type not present", __FUNCTION__);
	}
	state_to_go = data->getInt32Value(CMD_PS_MODE_TYPE);
	if(state_to_go>1) {
		throw chaos::CException(1, "Requeste mode type not implemented", __FUNCTION__);
	}
		
	getState(state, state_str);
	switch (state_to_go) {
		case 0://to standby
			//i need to be in operational to exec
			CMDCU_ << "Going to stanby";
			if((state != common::powersupply::POWER_SUPPLY_STATE_OPEN) &&
			   (state != common::powersupply::POWER_SUPPLY_STATE_ON)) {
				throw chaos::CException(2, boost::str( boost::format("Cant go to standby, current state is %1%[%2%]") % state_str % state), std::string(__FUNCTION__));
			}
			CMDCU_ << "Gone to stanby";
			break;
			
		case 1://to operational
			CMDCU_ << "Going to operational";
			if((state != common::powersupply::POWER_SUPPLY_STATE_STANDBY)) {
				throw chaos::CException(3, boost::str( boost::format("Cant go to operational, current state is %1%[%2%]") % state_str % state), std::string(__FUNCTION__));
			}
			CMDCU_ << "Gone to operational";
			break;
	}
	
	
	//set comamnd timeout for this instance
	setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	
	//send comamnd to driver
	powersupply_drv->poweron();
}

void own::CmdPSMode::ccHandler() {
	int state_id;
	std::string state_str;
	getState(state_id, state_str);
	
	switch(state_to_go) {
		case 0://we need to go in stanby
			if(state_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
				//we are terminated the command
				std::string error =  boost::str( boost::format("State reached %1% [%2%] we end command") % state_str % state_id);
				CMDCU_ << error;
				SL_END_RUNNIG_STATE
				return;
			}
			break;
			
		case 1://we need to go on operational
			if(state_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY ||
			   state_id == common::powersupply::POWER_SUPPLY_STATE_ON) {
				//we are terminated the command
				std::string error =  boost::str( boost::format("State reached %1% [%2%] we end command") % state_str % state_id);
				CMDCU_ << error;
				SL_END_RUNNIG_STATE
				return;
			}
			break;
	}
	
	
	if(state_id == common::powersupply::POWER_SUPPLY_STATE_ALARM ||
	   state_id == common::powersupply::POWER_SUPPLY_STATE_ERROR ||
	   state_id == common::powersupply::POWER_SUPPLY_STATE_UKN ) {
		std::string error =  boost::str( boost::format("Bad state got = %1% - [%2%]") % state_id % state_str);
		writeErrorMessage(error);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}
}

bool own::CmdPSMode::timeoutHandler() {
	//move the state machine on fault
	std::string error =  "Command operation has gone on timeout";
	writeErrorMessage(error);
	throw chaos::CException(1, error.c_str(), __FUNCTION__);
	return true;
}