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

using namespace driver::powersupply;
namespace chaos_sc = chaos::cu::control_manager::slow_command;

void AbstractPowerSupplyCommand::setHandler(c_data::CDataWrapper *data) {
	//get pointer to the output datase variable
	o_current = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)0)->getCurrentValue<double>();
	o_current_sp = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)1)->getCurrentValue<double>();
	o_voltage = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)2)->getCurrentValue<double>();
	o_polarity = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)3)->getCurrentValue<int32_t>();
	o_alarms = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)4)->getCurrentValue<uint64_t>();
	o_status = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)5)->getCurrentValue<char*>();
	o_dev_state = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)6)->getCurrentValue<uint64_t>();
	o_ps_state = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)7)->getCurrentValue<char*>();
	
	//get pointer to the input datase variable
	i_slope_up = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)0)->getCurrentValue<double>();
	i_slope_down = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)1)->getCurrentValue<double>();
	i_command_timeout = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)2)->getCurrentValue<uint32_t>();
	i_driver_timeout = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)2)->getCurrentValue<uint32_t>();
	//get pointer to the custom shared variable
	ps_state_machine_ptr = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_CUSTOM, (chaos_sc::VariableIndexType)0)->getCurrentValue< boost::msm::back::state_machine< powersupply_state_machine_impl > >();
	
	chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = driverAccessorsErogator->getAccessoInstanceByIndex(0);
	powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
}

// return the implemented handler
uint8_t AbstractPowerSupplyCommand::implementedHandler() {
	return  chaos_sc::HandlerType::HT_Set;
}

const char * AbstractPowerSupplyCommand::getCurrentState() {
	return  get_state_name(ps_state_machine_ptr->current_state()[0]);
}