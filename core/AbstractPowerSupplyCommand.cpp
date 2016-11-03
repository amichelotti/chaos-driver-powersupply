/*
 *	AbstractPowerSupplyCommand.cpp
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

#include "AbstractPowerSupplyCommand.h"
#include <boost/format.hpp>
#define CMDCUINFO_ INFO_LOG(AbstractPowerSupplyCommand)
#define CMDCUDBG_ DBG_LOG(AbstractPowerSupplyCommand)
#define CMDCUERR_ ERR_LOG(AbstractPowerSupplyCommand)

using namespace driver::powersupply;
namespace chaos_batch = chaos::common::batch_command;

AbstractPowerSupplyCommand::AbstractPowerSupplyCommand() {
  powersupply_drv = NULL;
}
AbstractPowerSupplyCommand::~AbstractPowerSupplyCommand() {
  if(powersupply_drv)
    delete (powersupply_drv);
  powersupply_drv = NULL;
}

void AbstractPowerSupplyCommand::setHandler(c_data::CDataWrapper *data) {
	CMDCUDBG_ << "setting ";
    o_stby=getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "stby");
    o_local=getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "local");
    o_pol=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");
    o_alarms = getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "alarms");
    o_current =getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"current");
    o_voltage =getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"voltage");

    i_asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampUpRate");
    i_asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampDownRate");
    c_polFromSet =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "polFromSet");
    c_polSwSign =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "polSwSign");
    c_stbyOnPol =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "stbyOnPol");
    c_zeroOnStby =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "zeroOnStby");
    s_bypass =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "bypass");
    p_minimumWorkingValue = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "minimumWorkingValue");
    p_maximumWorkingValue = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "maximumWorkingValue");
    p_warningThreshold = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "warningThreshold");
    p_warningThresholdTimeout = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "warningThresholdTimeout");
    p_resolution = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "resolution");

    
	//get pointer to the output datase variable
    chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = *s_bypass&&(driverAccessorsErogator->getAccessoInstanceByIndex(1))?driverAccessorsErogator->getAccessoInstanceByIndex(1):driverAccessorsErogator->getAccessoInstanceByIndex(0);
	if(power_supply_accessor != NULL) {
	  if(powersupply_drv == NULL){
	    powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
	  }
	}
}

// return the implemented handler
uint8_t AbstractPowerSupplyCommand::implementedHandler() {
	return  chaos_batch::HandlerType::HT_Set |chaos_batch::HandlerType::HT_Acquisition;
}

void AbstractPowerSupplyCommand::acquireHandler() {
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

void AbstractPowerSupplyCommand::getState(int& current_state, std::string& current_state_str) {
	CHAOS_ASSERT(powersupply_drv)
	int err = 0;
	std::string state_str;
	int32_t i_driver_timeout = getAttributeCache()->getValue<int32_t>(DOMAIN_INPUT, "driver_timeout");
	if((err=powersupply_drv->getState(&current_state, state_str, i_driver_timeout?i_driver_timeout:10000)) != 0) {
		setWorkState(false);
		CMDCUERR_ << boost::str( boost::format("Error getting the powersupply state = %1% ") % err);
	}

}

void AbstractPowerSupplyCommand::setWorkState(bool working_flag) {
	int64_t *o_dev_state = getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT, "dev_state");
	*o_dev_state = working_flag;
}
