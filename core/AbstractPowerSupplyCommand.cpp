/*
 *	AbstractPowerSupplyCommand.cpp
 *	!CHOAS
 *	Created by Andrea Michelotti.
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
#define CMDCUINFO_ INFO_LOG(AbstractPowerSupplyCommand) <<"["<<getDeviceID()<<"]"
#define CMDCUDBG_ DBG_LOG(AbstractPowerSupplyCommand) <<"["<<getDeviceID()<<"]"
#define CMDCUERR_ ERR_LOG(AbstractPowerSupplyCommand) <<"["<<getDeviceID()<<"]"

using namespace driver::powersupply;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
using namespace common::powersupply;

AbstractPowerSupplyCommand::AbstractPowerSupplyCommand():powersupply_drv(NULL) {
}
AbstractPowerSupplyCommand::~AbstractPowerSupplyCommand() {

	if(powersupply_drv)
		delete (powersupply_drv);
	powersupply_drv = NULL;
}


void AbstractPowerSupplyCommand::setHandler(c_data::CDataWrapper *data) {
	CMDCUDBG_ << "setting ";
	o_stby=getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "stby");
	i_stby=getAttributeCache()->getRWPtr<bool>(DOMAIN_INPUT, "stby");

	o_local=getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "local");
	i_local=getAttributeCache()->getRWPtr<bool>(DOMAIN_INPUT, "local");

	o_pol=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");
	i_pol=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "polarity");

	o_alarms = getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "alarms");
	o_current =getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"current");
	o_voltage =getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"voltage");
	o_off=getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT,"off");
	i_asup = getAttributeCache()->getRWPtr<double>(DOMAIN_INPUT, "rampUpRate");
	i_asdown = getAttributeCache()->getRWPtr<double>(DOMAIN_INPUT, "rampDownRate");
	i_current = getAttributeCache()->getRWPtr<double>(DOMAIN_INPUT, "current");

	c_polFromSet =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "polFromSet");
	c_polSwSign =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "polSwSign");
	c_stbyOnPol =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "stbyOnPol");
	c_zeroOnStby =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "zeroOnStby");
	//	s_bypass =getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "bypass");
	p_minimumWorkingValue = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "minimumWorkingValue");
	p_maximumWorkingValue = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "maximumWorkingValue");
	p_warningThreshold = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "warningThreshold");
	p_warningThresholdTimeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "warningThresholdTimeout");
	p_setTimeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "setTimeout");
	p_getTimeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "getTimeout");
	p_driverTimeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "driverTimeout");


	p_resolution = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "resolution");


	//get pointer to the output datase variable
	chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = (driverAccessorsErogator->getAccessoInstanceByIndex(1))?driverAccessorsErogator->getAccessoInstanceByIndex(1):driverAccessorsErogator->getAccessoInstanceByIndex(0);
	if(power_supply_accessor != NULL) {
		if(powersupply_drv == NULL){
			powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
		}
	}
	*o_alarms=0;
}

// return the implemented handler
uint8_t AbstractPowerSupplyCommand::implementedHandler() {
	return  chaos_batch::HandlerType::HT_Set |chaos_batch::HandlerType::HT_Acquisition|chaos_batch::HandlerType::HT_Correlation;;
}

void AbstractPowerSupplyCommand::acquireHandler() {
	int err;
	float tmp_float;
	int32_t tmp_int32;
	uint64_t tmp_uint64;
	chaos::common::alarm::MultiSeverityAlarmLevel level_stat;
	int driver_error=0;

	if((err = powersupply_drv->getCurrentOutput(&tmp_float))==0){
		*o_current = (double)tmp_float;
	} else {
		driver_error++;
		CMDCUERR_<<"Error getting current err:"<<err;
		if(err==POWER_SUPPLY_TIMEOUT){
			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Timeout Error calling driver on get current readout with code %1%") % err));

			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
		} else {
			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error calling driver on get current readout with code %1%") % err));

			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		}
	}
	if((err = powersupply_drv->getVoltageOutput(&tmp_float)) == 0){
		*o_voltage = (double)tmp_float;
	} else {
		driver_error++;
		CMDCUERR_<<"Error getting voltage err:"<<err;

		if(err==POWER_SUPPLY_TIMEOUT){
			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Timeout Error calling driver on get voltage readout with code %1%") % err));
			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		} else {

			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error calling driver on get voltage readout with code %1%") % err));

			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		}
	}


	if (powersupply_drv->getFeatures() & common::powersupply::POWER_SUPPLY_FEAT_MONOPOLAR)
{
    
	if((err = powersupply_drv->getPolarity(&tmp_int32)) == 0){
		*o_pol = tmp_int32;
	} else {
		driver_error++;
		CMDCUERR_<<"Error getting feature err:"<<err;

		if(err==POWER_SUPPLY_TIMEOUT){
			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Timeout Error calling driver on get polarity readout with code %1%") % err));
			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		} else {

			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error calling driver on get polarity readout with code %1%") % err));
			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		}
	}
}

	if((err = powersupply_drv->getAlarms(&tmp_uint64)) == 0){
		*o_alarms = tmp_uint64;
	} else {
		driver_error++;
		CMDCUERR_<<"Error getting alarms err:"<<err;

		if(err==POWER_SUPPLY_TIMEOUT){

			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Timeout Error calling driver on get alarms readout with code %1%") % err));
			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		} else {

			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error calling driver on get alarms readout with code %1%") % err) );
			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		}
		driver_error++;

	}

	std::string desc;
	if((err = powersupply_drv->getState(&state, desc,*p_driverTimeout)) == 0){
		*o_stby = (state & common::powersupply::POWER_SUPPLY_STATE_STANDBY)?true:false;
		*o_local= (state & common::powersupply::POWER_SUPPLY_STATE_LOCAL)?true:false;
		*o_off=(state & common::powersupply::POWER_SUPPLY_STATE_OFF)?true:false;
		if(*o_alarms){
			CMDCUDBG_<<"alarms!! "<<*o_alarms<<" desc:"<<desc;
		}
	} else {
		driver_error++;
		CMDCUERR_<<"Error getting state err:"<<err;

		if(err==POWER_SUPPLY_TIMEOUT){

			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Timeout Error calling driver on get state readout with code %1%") % err));
			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		} else {
			if(getStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error",level_stat) && (level_stat!=chaos::common::alarm::MultiSeverityAlarmLevelHigh)){
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error calling driver on get state readout with code %1%") % err) );
			}
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		}
	}

	if(driver_error==0){
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_error", chaos::common::alarm::MultiSeverityAlarmLevelClear);
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_timeout", chaos::common::alarm::MultiSeverityAlarmLevelClear);

	}
	CMDCUDBG_ << "current:" << *o_current<< ",current_sp:" << *i_current<< ",polarity:" << *o_pol<< ",alarms:" << *o_alarms<< ",stby -> " << *o_stby;

	//force output dataset as changed
}
void AbstractPowerSupplyCommand::ccHandler() {
	getAttributeCache()->setOutputDomainAsChanged();

}
void AbstractPowerSupplyCommand::getState(int& current_state, std::string& current_state_str) {
	CHAOS_ASSERT(powersupply_drv)
																			int err = 0;
	std::string state_str;
	if((err=powersupply_drv->getState(&current_state, state_str, *p_getTimeout?*p_getTimeout:10000)) != 0) {
		CMDCUERR_ << boost::str( boost::format("Error getting the powersupply state = %1% ") % err);
	}

}
