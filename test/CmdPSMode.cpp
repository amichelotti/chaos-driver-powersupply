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
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSMode::setHandler(c_data::CDataWrapper *data) {
	CMDCU_ << "Executing set handler";
	SL_EXEC_RUNNIG_STATE
	AbstractPowerSupplyCommand::setHandler(data);

	//requested mode
	if(!data->hasKey(CMD_PS_MODE_TYPE)) {
		throw chaos::CException(0, "Mode type not present", __FUNCTION__);
	}
	state_to_go = data->getInt32Value(CMD_PS_MODE_TYPE);
	if(state_to_go>1) {
		throw chaos::CException(1, "Requeste mode type not implemented", __FUNCTION__);
	}
		
	switch (state_to_go) {
		case 0://to standby
			//i need to be in operational to exec
			CMDCU_ << "Request to go to stanby";
			if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_OPEN) &&
			   (*o_status_id != common::powersupply::POWER_SUPPLY_STATE_ON)) {
				throw chaos::CException(2, boost::str( boost::format("Cant go to standby, current state is %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
			}
			if(powersupply_drv->standby() != 0) {
				throw chaos::CException(3, "Error issuing standby on powersupply", __FUNCTION__);
			}
			CMDCU_ << "Can go to stanby";
			break;
			
		case 1://to operational
			CMDCU_ << "Request to go to operational";
			if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_STANDBY)) {
				throw chaos::CException(3, boost::str( boost::format("Cant go to operational, current state is %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
			}
			if(powersupply_drv->poweron() != 0) {
				throw chaos::CException(3, "Error issuing poweron on powersupply", __FUNCTION__);
			}
			CMDCU_ << "Can go to operational";
			break;
	}
	
	
	//set comamnd timeout for this instance
	if(*i_command_timeout) {
		CMDCU_ << "Set time out in "<< *i_command_timeout << "milliseconds";
		//we have a timeout for command so apply it to this instance
		setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	//send comamnd to driver
	setWorkState(true);
}

void own::CmdPSMode::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
	
	SL_EXEC_RUNNIG_STATE
	CMDCU_ << "Check if we are gone";
	switch(state_to_go) {
		case 0://we need to go in stanby
			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
				setWorkState(false);
				//we are terminated the command
				CMDCU_ << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
				SL_END_RUNNIG_STATE
				return;
			}
			break;
			
		case 1://we need to go on operational
			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY ||
			   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ON) {
				setWorkState(false);
				//we are terminated the command
				CMDCU_ << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
				SL_END_RUNNIG_STATE
				return;
			}
			break;
	}
	
	
	if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ALARM ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ERROR ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_UKN ) {
		setWorkState(false);
		std::string error =  boost::str( boost::format("Bad state got = %1% - [%2%]") % *o_status_id % o_status);
		writeErrorMessage(error);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}
}

bool own::CmdPSMode::timeoutHandler() {
	//move the state machine on fault
	setWorkState(false);
	std::string error =  "Command operation has gone on timeout";
	writeErrorMessage(error);
	throw chaos::CException(1, error.c_str(), __FUNCTION__);
	return true;
}