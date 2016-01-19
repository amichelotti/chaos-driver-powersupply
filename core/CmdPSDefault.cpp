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

#include <string.h>
#include "CmdPSDefault.h"
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

#define LOG_HEAD_CmdPSDefault LOG_TAIL(CmdPSDefault)

#define CMDCU_ INFO_LOG(CmdPSDefault)
#define CMDCUDBG_ DBG_LOG(CmdPSDefault)
#define CMDCUERR_ ERR_LOG(CmdPSDefault)

using namespace driver::powersupply;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;


BATCH_COMMAND_OPEN_DESCRIPTION(driver::powersupply::,CmdPSDefault,
                                                          "Default method",
                                                          "4403148a-35df-11e5-b1f7-7f8214ad6212")
BATCH_COMMAND_CLOSE_DESCRIPTION()
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
	
	AbstractPowerSupplyCommand::setHandler(data);

	//set the default scheduling to one seconds
	setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);

	//get channel pointer
	o_current = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "current");
	o_current_sp = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "current_sp");
	o_voltage = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "voltage");
	o_polarity = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");
	o_dev_state = getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "dev_state");
	o_on = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "on");
	o_stby = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "stby");
	o_alarm = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "alarm");

	BC_NORMAL_RUNNIG_PROPERTY
    sequence_number = 0;
	slow_acquisition_idx = 0;
}

    // Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void CmdPSDefault::acquireHandler() {
	string desc;
	int err = 0;
	int stato = 0;
	float tmp_float = 0.0F;
	int tmp_uint32 = 0;
	uint64_t tmp_uint64 = 0;
	CMDCU_ << "Acquiring data";
	
	boost::shared_ptr<SharedCacheLockDomain> r_lock = getAttributeCache()->getLockOnCustomAttributeCache();
	r_lock->lock();
	
    if((err = powersupply_drv->getCurrentOutput(&tmp_float))==0){
		*o_current = (double)tmp_float;
    } else {
		LOG_AND_TROW(CMDCUERR_, 1, boost::str( boost::format("Error calling driver on get current readout with code %1%") % err));
	}

	if((err = powersupply_drv->getVoltageOutput(&tmp_float)) == 0){
		*o_voltage = (double)tmp_float;
	} else {
		LOG_AND_TROW(CMDCUERR_, 2, boost::str( boost::format("Error calling driver on get voltage readout with code %1%") % err));
	}

	if((err = powersupply_drv->getPolarity(&tmp_uint32)) == 0){
		*o_polarity = tmp_uint32;
	} else {
		LOG_AND_TROW(CMDCUERR_, 3, boost::str( boost::format("Error calling driver on get polarity readout with code %1%") % err));
	}

	if((err = powersupply_drv->getAlarms(&tmp_uint64)) == 0){
		*o_alarms = tmp_uint64;
	} else {
		LOG_AND_TROW(CMDCUERR_, 4, boost::str( boost::format("Error calling driver on get alarms readout with code %1%") % err));
	}

	if((err = powersupply_drv->getState(&stato, desc)) == 0){
		*o_status_id = stato;
		//update the value and dimension of status channel
		//getAttributeCache()->setOutputAttributeValue("status", (void*)desc.c_str(), (uint32_t)desc.size());
		//the new pointer need to be got (set new size can reallocate the pointer)
		o_status = getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "status");
		//copy up to 255 and put the termination character
		strncpy(o_status, desc.c_str(), 256);
	} else {
		LOG_AND_TROW(CMDCUERR_, 5, boost::str( boost::format("Error calling driver on get state readout with code %1%") % err));
	}

    CMDCU_ << "current ->" << *o_current;
    CMDCU_ << "current_sp ->" << *o_current_sp;
    CMDCU_ << "voltage ->" << *o_voltage;
    CMDCU_ << "polarity ->" << *o_polarity;
    CMDCU_ << "alarms ->" << *o_alarms;
    CMDCU_ << "status_id -> " << *o_status_id;
	
	/*
	 * Javascript Interface
	 */
	*o_on = (*o_status_id & common::powersupply::POWER_SUPPLY_STATE_ON) ? 1:0;
	*o_stby = (*o_status_id & common::powersupply::POWER_SUPPLY_STATE_STANDBY)?1:0;
	*o_alarm = (*o_alarms!=0)?1:0;

   
	CMDCU_ << "stby =>"<<((*o_status_id & common::powersupply::POWER_SUPPLY_STATE_STANDBY)?1:0);
    CMDCU_ << "status. -> " << o_status;
    CMDCU_ << "dev_state -> " << *o_dev_state;
    CMDCU_ << "sequence_number -> " << sequence_number;
	
	//force output dataset as changed
	getAttributeCache()->setOutputDomainAsChanged();
}
