//
//  CmdPSMode.cpp
//  PowerSupply
//
//  Created by Claudio Bisegni on 06/11/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdPSMode.h"

#include <boost/format.hpp>

#define CMDCUINFO INFO_LOG(CmdPSMode) << "[" << getDeviceID() << "] "
#define CMDCUDBG DBG_LOG(CmdPSMode) << "[" << getDeviceID() << "] "
#define CMDCUERR ERR_LOG(CmdPSMode) << "[" << getDeviceID() << "] "

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
	
	int err = 0;
	int state = 0;
	std::string state_description;

	AbstractPowerSupplyCommand::setHandler(data);
        AbstractPowerSupplyCommand::acquireHandler();


	//requested mode
	if(!data->hasKey(CMD_PS_MODE_TYPE)) {
		CMDCUERR << "Mode type not present";
		BC_END_RUNNING_PROPERTY;
		return;
	}
	state_to_go = data->getInt32Value(CMD_PS_MODE_TYPE);
	if(state_to_go>1) {
		CMDCUERR << "Request mode type not implemented";
		BC_END_RUNNING_PROPERTY;
		return;
	}
        
	


	switch (state_to_go) {
		case 0://to standby
			//i need to be in operational to exec
			CMDCUINFO << "Request to go to stanby";
			 
                    if((err = powersupply_drv->standby())) {
			CMDCUERR<<boost::str( boost::format("Error calling driver for standby on powersupply") % err);
                        BC_END_RUNNING_PROPERTY;
                        return;
                       }
                        *i_stby=true;
			break;
			
		case 1://to operational
		   
                    CMDCUINFO << "Request to go to operational";
                
                
                    if((err = powersupply_drv->poweron())) {
			CMDCUERR<<boost::str( boost::format("Error calling driver for operation on powersupply") % err);
                        BC_END_RUNNING_PROPERTY;
                        return;
                
                    }
                    *i_stby=false;

			break;
	}
 
	//set comamnd timeout for this instance
	if(*p_setTimeout) {
		CMDCUINFO << "Set time out in "<< *p_setTimeout << "milliseconds";
		//we have a timeout for command so apply it to this instance
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *p_setTimeout);
	} else {
		CMDCUINFO << "Set time out in milliseconds "<<DEFAULT_COMMAND_TIMEOUT_MS;
		//we have a timeout for command so apply it to this instance
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)DEFAULT_COMMAND_TIMEOUT_MS);
	}
	
	//send comamnd to driver
	setWorkState(true);
        getAttributeCache()->setInputDomainAsChanged();
//        pushInputDataset();
	//run in esclusive mode
	BC_EXEC_RUNNING_PROPERTY;
}

void own::CmdPSMode::acquireHandler() {
            AbstractPowerSupplyCommand::acquireHandler();

	//force output dataset as changed
	getAttributeCache()->setOutputDomainAsChanged();
}

void own::CmdPSMode::ccHandler() {
	CMDCUINFO << "enter acquireHandler";
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	CMDCUINFO << "Check if we are gone";
	
        
        if(*i_stby==*o_stby){
            CMDCUINFO<<"STATE reached stby:"<<*i_stby;
            BC_END_RUNNING_PROPERTY
            setWorkState(false);
            return;
        }
	if(*o_alarms) {
		BC_END_RUNNING_PROPERTY
		setWorkState(false);
		CMDCUERR << boost::str(boost::format("[metric] Got alarm code %1% in %3% milliseconds") % *o_alarms % elapsed_msec);
	}
	CMDCUINFO << "exit acquireHandler";
}

bool own::CmdPSMode::timeoutHandler() {
	CMDCUINFO << "enter timeoutHandler";
	//move the state machine on fault
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	setWorkState(false);
         if(*i_stby==*o_stby){
            CMDCUINFO<<"STATE reached stby:"<<*i_stby;
            BC_END_RUNNING_PROPERTY
            setWorkState(false);
            return false;
        }
	BC_END_RUNNING_PROPERTY

	CMDCUINFO << "timeout";
	return true;
}
