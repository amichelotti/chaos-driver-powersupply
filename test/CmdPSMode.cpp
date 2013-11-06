//
//  CmdPSMode.cpp
//  PowerSupply
//
//  Created by Claudio Bisegni on 06/11/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdPSMode.h"

#include <boost/format.hpp>

#define CMDCU_ LAPP_ << "[DefaultAcquisitionCommand] - "

namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;

// return the implemented handler
uint8_t own::CmdPSMode::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler() |
	ccc_slow_command::HandlerType::HT_Correlation;
}

void own::CmdPSMode::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	
	//requested mode
	if(!data->hasKey(CMD_PS_MODE_TYPE)) {
		throw chaos::CException(0, "Mode type not present", __FUNCTION__);
	}
	int32_t tmp_int = data->getInt32Value(CMD_PS_MODE_TYPE);
	if(tmp_int>1) {
		throw chaos::CException(1, "Requeste mode type not implemented", __FUNCTION__);
	}
	own::PowersupplyEventType::ModeStart request_mode = (own::PowersupplyEventType::ModeStart)tmp_int;
	
	//update state machine
	if(ps_state_machine_ptr->process_event(own::PowersupplyEventType::mode_start(request_mode) ) == boost::msm::back::HANDLED_TRUE) {
		throw chaos::CException(2, "Current state doesn't permite set the mode ", __FUNCTION__);
	}
	
	//set comamnd timeout for this instance
	setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	
	//send comamnd to driver
	powersupply_drv->poweron();
}

void own::CmdPSMode::ccHandler() {
	int state_id;
	int err = 0;
	std::string state_str;
	try {
		//check timeout and powersupply state
		if((err=powersupply_drv->getState(&state_id, state_str)) != 0) {
			std::string error =  boost::str( boost::format("Error getting the powersupply state = %1% ") % err);
			throw chaos::CException(1, error.c_str(), __FUNCTION__);
		}
	} catch (...) {
		throw chaos::CException(2, "Get state command has forward an exception", __FUNCTION__);
	}
	
	if(state_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
		//we are terminated the command
		SL_END_RUNNIG_STATE
		return;
	}
	
	if(state_id != common::powersupply::POWER_SUPPLY_STATE_ON) {
		std::string error =  boost::str( boost::format("Bad state got = %1% - [%2%]") % state_id % state_str);
		throw chaos::CException(2, error.c_str(), __FUNCTION__);
	}
}

bool own::CmdPSMode::timeoutHandler() {
	//move the state machine on fault
	if(ps_state_machine_ptr->process_event(own::PowersupplyEventType::fault()) == boost::msm::back::HANDLED_TRUE) {
		throw chaos::CException(0, "Error going to fault state", __FUNCTION__);
	}
	return true;
}