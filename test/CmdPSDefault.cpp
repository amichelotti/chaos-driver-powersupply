/*
 *	CmdPSDefault.cpp
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

#include <cstring>
#include "CmdPSDefault.h"

#define CMDCU_ LAPP_ << "[CmdPSDefault] - "
#define CMDCUDBG_ LDBG_ << "[CmdPSDefault] - "

using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace driver::powersupply;


CmdPSDefault::CmdPSDefault() {
	powersupply_drv = NULL;
}

CmdPSDefault::~CmdPSDefault() {
	
}

// return the implemented handler
uint8_t CmdPSDefault::implementedHandler() {
	//add to default hadnler the acquisition one
	return  AbstractPowerSupplyCommand::implementedHandler() |
			HandlerType::HT_Acquisition;
}

// Start the command execution
void CmdPSDefault::setHandler(c_data::CDataWrapper *data) {
	//set command has stackable
	CMDCU_ << "Change running property to SL_STACK_RUNNIG_STATE";

	//call superclass set handler to setup all variable
	AbstractPowerSupplyCommand::setHandler(data);

	//no adiditonal setup here
	SL_STACK_RUNNIG_STATE
    
    sequence_number = 0;
}

// Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void CmdPSDefault::acquireHandler() {
	string desc;
	int stato = 0;
	float tmp_float = 0.0F;
	int tmp_uint32 = 0;
	uint64_t tmp_uint64 = 0;
	CMDCU_ << "Acquiring data";

	
    CDataWrapper *acquiredData = getNewDataWrapper();
	
    if(powersupply_drv && !powersupply_drv->getCurrentOutput(&tmp_float)){
		*o_current = (double)tmp_float;
    }
	
	if(powersupply_drv && !powersupply_drv->getVoltageOutput(&tmp_float)){
		*o_voltage = (double)tmp_float;
    }
    
    if(powersupply_drv && !powersupply_drv->getPolarity(&tmp_uint32)){
		*o_polarity = tmp_uint32;
    }
	
	
    if(powersupply_drv && !powersupply_drv->getAlarms(&tmp_uint64)){
		*o_alarms = tmp_uint64;
    }
	
    if(powersupply_drv && !powersupply_drv->getState(&stato, desc)){
		CMDCU_ << "got state ->" << desc << "[" << stato << "]";
		std::strncpy(o_status, desc.c_str(), 256);
    }
    acquiredData->addDoubleValue("current", *o_current);
	acquiredData->addDoubleValue("current_sp", *o_current_sp);
    acquiredData->addDoubleValue("voltage", *o_voltage);
    acquiredData->addInt32Value("polarity", *o_polarity);
    acquiredData->addInt64Value("alarms", *o_alarms);
    acquiredData->addInt32Value("status_id", (*o_status_id = stato));
    acquiredData->addStringValue("status", o_status);
	//set the current device state and last error
	acquiredData->addInt64Value("dev_state", *o_dev_state);
	acquiredData->addStringValue("cmd_last_error", o_cmd_last_error);
	acquiredData->addInt64Value("seq", sequence_number++);
    //push data on central cache
	pushDataSet(acquiredData);
}