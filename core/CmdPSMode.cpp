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

//using  namespace driver::powersupply;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSMode,CMD_PS_MODE_ALIAS,
                                                          "Set powersupply mode (on,standby)",
                                                          "34c5dc7e-35ca-11e5-a7ed-971f57b4b945")
BATCH_COMMAND_ADD_INT32_PARAM(CMD_PS_MODE_TYPE, "0:standby, 1:on",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()
// return the implemented handler
uint8_t own::CmdPSMode::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSMode::setHandler(c_data::CDataWrapper *data) {
	CMDCU_ << "Executing set handler";
	BC_EXEC_RUNNIG_PROPERTY
	AbstractPowerSupplyCommand::setHandler(data);
	i_command_timeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "command_timeout");

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
				CHAOS_EXCEPTION(2, boost::str( boost::format("Can't go to standby, current state is %1%[%2%]") % o_status % *o_status_id));
			}
			if(powersupply_drv->standby() != 0) {
				CHAOS_EXCEPTION(3, "Error issuing standby on powersupply");
			}
			CMDCU_ << "Can go to stanby";
			break;
			
		case 1://to operational
			CMDCU_ << "Request to go to operational";
			if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_STANDBY)) {
				CHAOS_EXCEPTION(3, boost::str( boost::format("Cant go to operational, current state is %1%[%2%]") % o_status % *o_status_id));
			}
			if(powersupply_drv->poweron() != 0) {
				CHAOS_EXCEPTION(5, "Error issuing poweron on powersupply");
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
		CHAOS_EXCEPTION(1, boost::str( boost::format("Bad state got = %1% - [%2%]") % *o_status_id % o_status));
	}
}

bool own::CmdPSMode::timeoutHandler() {
	//move the state machine on fault
	setWorkState(false);
	CHAOS_EXCEPTION(1, "Command operation has gone on timeout");
	return true;
}
