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
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

#define LOG_HEAD_CmdPSDefault LOG_TAIL(CmdPSDefault)

#define CMDCU_ LAPP_ << LOG_HEAD_CmdPSDefault
#define CMDCUDBG_ LDBG_ << LOG_HEAD_CmdPSDefault

using namespace driver::powersupply;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;

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
	string desc;
	int stato = 0;
	int tmp_uint32 = 0;
	float tmp_float = 0.0F;
	uint64_t tmp_uint64 = 0;
	
	AbstractPowerSupplyCommand::setHandler(data);
	
	setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
	
	if(!powersupply_drv->getCurrentOutput(&tmp_float)){
		*o_current = (double)tmp_float;
    }
	if(!powersupply_drv->getVoltageOutput(&tmp_float)){
		*o_voltage = (double)tmp_float;
	}
	if(!powersupply_drv->getPolarity(&tmp_uint32)){
		*o_polarity = tmp_uint32;
	}
	if(!powersupply_drv->getAlarms(&tmp_uint64)){
		*o_alarms = tmp_uint64;
	}
	if(!powersupply_drv->getState(&stato, desc)){
		*o_status_id = stato;
		std::strncpy(o_status, desc.c_str(), 256);
	}
	
        //set command has stackable
	CMDCU_ << "Change running property to SL_STACK_RUNNIG_STATE";
    

		//get the default value
	
	
        //no adiditonal setup here
	BC_NORMAL_RUNNIG_PROPERTY
    
    sequence_number = 0;
    last_slow_acq_time = getSetTime();// shared_stat->lastCmdStepStart;
	slow_acquisition_idx = 0;
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
    
    uint64_t time_diff = getStartStepTime() - last_slow_acq_time;
    
    
    CDataWrapper *acquiredData = getNewDataWrapper();
	
    if(powersupply_drv && !powersupply_drv->getCurrentOutput(&tmp_float)){
		*o_current = (double)tmp_float;
    }
	
    if(time_diff > 2000000 ) {
		CMDCU_ << "slow acquire staterd after us=" << time_diff;
        last_slow_acq_time = getStartStepTime();
		switch(slow_acquisition_idx) {
			case 0:
				if(powersupply_drv && !powersupply_drv->getVoltageOutput(&tmp_float)){
					*o_voltage = (double)tmp_float;
				}
				break;
			case 1:
				if(powersupply_drv && !powersupply_drv->getPolarity(&tmp_uint32)){
					*o_polarity = tmp_uint32;
				}
				break;
			case 2:
				if(powersupply_drv && !powersupply_drv->getAlarms(&tmp_uint64)){
					*o_alarms = tmp_uint64;
				}
				break;
			case 3:
				if(powersupply_drv && !powersupply_drv->getState(&stato, desc)){
					*o_status_id = stato;
					std::strncpy(o_status, desc.c_str(), 256);
				}
				break;
		}
		slow_acquisition_idx++;
		slow_acquisition_idx = slow_acquisition_idx % 4;
	}
    acquiredData->addDoubleValue("current", *o_current);
    CMDCU_ << "current ->" << *o_current;
	acquiredData->addDoubleValue("current_sp", *o_current_sp);
    CMDCU_ << "current_sp ->" << *o_current_sp;
    acquiredData->addDoubleValue("voltage", *o_voltage);
    CMDCU_ << "voltage ->" << *o_voltage;
    acquiredData->addInt32Value("polarity", *o_polarity);
    CMDCU_ << "polarity ->" << *o_polarity;
    acquiredData->addInt64Value("alarms", *o_alarms);
    CMDCU_ << "alarms ->" << *o_alarms;
    acquiredData->addInt32Value("status_id", *o_status_id );
    CMDCU_ << "status_id -> " << *o_status_id;
    acquiredData->addStringValue("status", o_status);
    /*
     * Javascript Interface
     */
    acquiredData->addInt32Value("on", (*o_status_id & common::powersupply::POWER_SUPPLY_STATE_ON) ? 1:0);
    acquiredData->addInt32Value("stby", (*o_status_id & common::powersupply::POWER_SUPPLY_STATE_STANDBY)?1:0);
    acquiredData->addInt32Value("alarm", (*o_alarms!=0)?1:0);
   
     CMDCU_ << "stby =>"<<((*o_status_id & common::powersupply::POWER_SUPPLY_STATE_STANDBY)?1:0);
 
    CMDCU_ << "status. -> " << o_status;
        //set the current device state and last error
	acquiredData->addInt64Value("dev_state", *o_dev_state);
    CMDCU_ << "dev_state -> " << *o_dev_state;
	acquiredData->addStringValue("cmd_last_error", o_cmd_last_error);
    CMDCU_ << "cmd_last_error -> " << o_cmd_last_error;
	acquiredData->addInt64Value("seq", sequence_number++);
    CMDCU_ << "sequence_number -> " << sequence_number;
        //push data on central cache
	pushDataSet(acquiredData);
}
