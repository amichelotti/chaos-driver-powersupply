//
//  CmdPSMode.cpp
//  PowerSupply
//
//  Created by Claudio Bisegni on 06/11/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdPSMode.h"

#include <boost/format.hpp>

#define LOG_HEAD_CmdPSMode LOG_TAIL(CmdPSMode)

#define CMDCU_ LAPP_ << LOG_HEAD_CmdPSMode

namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

// return the implemented handler
uint8_t own::CmdPSMode::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSMode::setHandler(c_data::CDataWrapper *data) {
	CMDCU_ << "Executing set handler";
	BC_EXEC_RUNNIG_PROPERTY
	AbstractPowerSupplyCommand::setHandler(data);
	i_command_timeout = getAttributeCache()->getROPtr<uint32_t>(AttributeValueSharedCache::SVD_INPUT, "command_timeout");

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
			if(*o_status_id &  common::powersupply::POWER_SUPPLY_STATE_STANDBY){
			  CMDCU_ << "Already in standby";
			} else if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_OPEN) &&
			   (*o_status_id != common::powersupply::POWER_SUPPLY_STATE_ON)) {
				TROW_ERROR(2, boost::str( boost::format("Can't go to standby, current state is %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
			}
			if(powersupply_drv->standby() != 0) {
				TROW_ERROR(3, "Error issuing standby on powersupply", std::string(__FUNCTION__))
			}
			CMDCU_ << "Can go to stanby";
			break;
			
		case 1://to operational
			CMDCU_ << "Request to go to operational";
			if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_STANDBY)) {
				TROW_ERROR(3, boost::str( boost::format("Cant go to operational, current state is %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
			}
			if(powersupply_drv->poweron() != 0) {
				TROW_ERROR(5, "Error issuing poweron on powersupply", std::string(__FUNCTION__))
			}
			CMDCU_ << "Can go to operational";
			break;
	}
	
	
	//set comamnd timeout for this instance
	if(*i_command_timeout) {
		CMDCU_ << "Set time out in "<< *i_command_timeout << "milliseconds";
		//we have a timeout for command so apply it to this instance
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	//send comamnd to driver
	setWorkState(true);
}

void own::CmdPSMode::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
	
	BC_EXEC_RUNNIG_PROPERTY
	CMDCU_ << "Check if we are gone";
	switch(state_to_go) {
		case 0://we need to go in stanby
			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
				setWorkState(false);
				//we are terminated the command
				CMDCU_ << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
				BC_END_RUNNIG_PROPERTY
				return;
			}
			break;
			
		case 1://we need to go on operational
			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY ||
			   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ON) {
				setWorkState(false);
				//we are terminated the command
				CMDCU_ << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
				BC_END_RUNNIG_PROPERTY
				return;
			}
			break;
	}
	
	
	if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ALARM ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ERROR ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_UKN ) {
		setWorkState(false);
		TROW_ERROR(1, boost::str( boost::format("Bad state got = %1% - [%2%]") % *o_status_id % o_status), std::string(__FUNCTION__))
	}
}

bool own::CmdPSMode::timeoutHandler() {
	//move the state machine on fault
	setWorkState(false);
	TROW_ERROR(1, "Command operation has gone on timeout", std::string(__FUNCTION__))
	return true;
}
