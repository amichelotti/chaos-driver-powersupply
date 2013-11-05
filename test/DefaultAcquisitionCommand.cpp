/*
 *	DefaultAcquisitionCommand.cpp
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
#include "DefaultAcquisitionCommand.h"

#define CMDCU_ LAPP_ << "[DefaultAcquisitionCommand] - "


using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace driver::powersupply;


DefaultAcquisitionCommand::DefaultAcquisitionCommand() {
	
}

DefaultAcquisitionCommand::~DefaultAcquisitionCommand() {
	
}

// return the implemented handler
uint8_t DefaultAcquisitionCommand::implementedHandler() {
	//add to default hadnler the acquisition one
	return  AbstractPowerSupplyCommand::implementedHandler() |
			HandlerType::HT_Acquisition;
}

// Start the command execution
void DefaultAcquisitionCommand::setHandler(c_data::CDataWrapper *data) {
	//call superclass set handler to setup all variable
	AbstractPowerSupplyCommand::setHandler(data);

	//no adiditonal setup here
}

// Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void DefaultAcquisitionCommand::acquireHandler() {
	string desc;
	int stato = 0;
	float tmp_float = 0.0F;
	int tmp_uint32 = 0;
	uint64_t tmp_uint64 = 0;
	
    CDataWrapper *acquiredData = getNewDataWrapper();
    if(!powersupply_drv->getVoltageOutput(&tmp_float)){
		*o_voltage = (double)tmp_float;
		acquiredData->addDoubleValue("voltage", *o_voltage);
    }

	
	acquiredData->addDoubleValue("current_sp", *o_current_sp);
	
    if(!powersupply_drv->getCurrentOutput(&tmp_float)){
		*o_current = (double)tmp_float;
		acquiredData->addDoubleValue("current", *o_current);
    }
    
	
    if(!powersupply_drv->getPolarity(&tmp_uint32)){
		*o_polarity = tmp_uint32;
		acquiredData->addInt32Value("polarity", *o_polarity);
    }
	
	
    if(!powersupply_drv->getAlarms(&tmp_uint64)){
		*o_alarms = tmp_uint64;
		acquiredData->addInt64Value("alarms", *o_alarms);
    }
	
    if(!powersupply_drv->getState(&stato, desc)){
		std::strncpy(*o_status, desc.c_str(), 256);
		acquiredData->addStringValue("o_status", *o_status);
    }
	
	//set the current device state
	
    //push data on central cache
	pushDataSet(acquiredData);
}