//
//  CmdPSMode.cpp
//  PowerSupply
//
//  Created by Claudio Bisegni on 06/11/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdPSMode.h"

#include <boost/format.hpp>

#define CMDCUINFO INFO_LOG(CmdPSMode)
#define CMDCUDBG DBG_LOG(CmdPSMode)
#define CMDCUERR ERR_LOG(CmdPSMode)

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
    return	AbstractPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}

void own::CmdPSMode::setHandler(c_data::CDataWrapper *data) {
	CMDCUINFO << "Executing set handler";
	BC_EXEC_RUNNIG_PROPERTY
	int err = 0;
	AbstractPowerSupplyCommand::setHandler(data);
	i_command_timeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "command_timeout");

	//requested mode
	if(!data->hasKey(CMD_PS_MODE_TYPE)) {
		CMDCUERR << "Mode type not present";
		BC_END_RUNNIG_PROPERTY;
		return;
	}
	state_to_go = data->getInt32Value(CMD_PS_MODE_TYPE);
	if(state_to_go>1) {
		CMDCUERR << "Requeste mode type not implemented";
		BC_END_RUNNIG_PROPERTY;
		return;
	}
		
	switch (state_to_go) {
		case 0://to standby
			//i need to be in operational to exec
			CMDCUINFO << "Request to go to stanby";
			if(*o_status_id &  common::powersupply::POWER_SUPPLY_STATE_STANDBY){
				CMDCUINFO << "Already in standby";
				BC_END_RUNNIG_PROPERTY
			} else if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_OPEN) &&
			   (*o_status_id != common::powersupply::POWER_SUPPLY_STATE_ON)) {
				CMDCUERR << boost::str( boost::format("Can't go to standby, current state is %1%[%2%]") % o_status % *o_status_id);
				BC_END_RUNNIG_PROPERTY;
			} else {
                CMDCUINFO << "Can go to stanby";
                if((err = powersupply_drv->standby())) {
					LOG_AND_TROW(CMDCUERR, 1, boost::str( boost::format("Error calling driver for standby on powersupply") % err));
                }
			}
			break;
			
		case 1://to operational
		    if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ON){
				CMDCUINFO << "Already in on";
				BC_END_RUNNIG_PROPERTY
			} else {
                CMDCUINFO << "Request to go to operational";
                if((*o_status_id != common::powersupply::POWER_SUPPLY_STATE_STANDBY)) {
					CMDCUERR << boost::str( boost::format("Cant go to operational, current state is %1%[%2%]") % o_status % *o_status_id);
					BC_END_RUNNIG_PROPERTY;
					return;
                }
                CMDCUINFO << "Can go to operational";
                if((err = powersupply_drv->poweron())) {
					LOG_AND_TROW(CMDCUERR, 2, boost::str( boost::format("Error calling driver for power on powersupply") % err));
                }
			}
			break;
	}
	
	
	//set comamnd timeout for this instance
	if(*i_command_timeout) {
		CMDCUINFO << "Set time out in "<< *i_command_timeout << "milliseconds";
		//we have a timeout for command so apply it to this instance
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	//send comamnd to driver
	setWorkState(true);
        // do not check for accomplishement
	BC_END_RUNNIG_PROPERTY;
}

void own::CmdPSMode::acquireHandler() {
	int err = 0;
	std::string state_description;
	int state = 0;
	//!get the only state because thsi command work only on it
	if((err = powersupply_drv->getState(&state, state_description))){
		LOG_AND_TROW(CMDCUERR, 1, boost::str( boost::format("Error calling driver for get state from powersupply") % err));
	} else {
		*o_status_id = state;
		//copy up to 255 and put the termination character
		strncpy(o_status, state_description.c_str(), 256);
	}

	if((err = powersupply_drv->getAlarms(o_alarms))){
		LOG_AND_TROW(CMDCUERR, 2, boost::str( boost::format("Error calling driver for get alarms from powersupply") % err));
	}
	//force output dataset as changed
	getAttributeCache()->setOutputDomainAsChanged();
}

void own::CmdPSMode::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
#if 0
	CMDCUINFO << "Check if we are gone";
	switch(state_to_go) {
		case 0://we need to go in stanby
			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
				setWorkState(false);
				//we are terminated the command
				CMDCUINFO << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
				BC_END_RUNNIG_PROPERTY
				return;
			}
			break;
			
		case 1://we need to go on operational
			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ON) {
				setWorkState(false);
				//we are terminated the command
				CMDCUINFO << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
				BC_END_RUNNIG_PROPERTY
				return;
			}
			break;
	}
	
	
	if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ALARM ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ERROR ||
	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_UKN) {
		setWorkState(false);
		CMDCUERR << boost::str( boost::format("Bad state got = %1% - [%2%]") % *o_status_id % o_status);
	}
#endif
}

bool own::CmdPSMode::timeoutHandler() {
	//move the state machine on fault
	setWorkState(false);
	return true;
}
