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
#define CMDCUDBG_ LDBG_ << "[AbstractPowerSupplyCommand] - "
#include "AbstractPowerSupplyCommand.h"
#include <boost/format.hpp>

using namespace driver::powersupply;
namespace chaos_sc = chaos::cu::control_manager::slow_command;

void AbstractPowerSupplyCommand::setHandler(c_data::CDataWrapper *data) {
	CMDCUDBG_ << "loading pointer for output channel";
	int idx = 0;
	//get pointer to the output datase variable
	o_current = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<double>();
	o_current_sp = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<double>();
	o_voltage = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<double>();
	o_polarity = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<int32_t>();
	o_alarms = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<uint64_t>();
	o_status_id = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<int32_t>();
	o_status = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<char>();
	o_dev_state = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<uint64_t>();
	o_cmd_last_error = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_OUTPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<char>();
	
	//get pointer to the input datase variable
	idx = 0;
	i_slope_up = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<double>();
	i_slope_down = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<double>();
	i_command_timeout = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<uint32_t>();
	i_driver_timeout = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<uint32_t>();
	i_delta_setpoint = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<uint32_t>();
	i_setpoint_affinity = getVariableValue(chaos_sc::IOCAttributeShareCache::SVD_INPUT, (chaos_sc::VariableIndexType)idx++)->getCurrentValue<uint32_t>();
	
	chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = driverAccessorsErogator->getAccessoInstanceByIndex(0);
	if(power_supply_accessor != NULL) {
		powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
	}
	//clean the last error
	writeErrorMessage("");
}

// return the implemented handler
uint8_t AbstractPowerSupplyCommand::implementedHandler() {
	return  chaos_sc::HandlerType::HT_Set |
			chaos_sc::HandlerType::HT_Correlation;
}

void AbstractPowerSupplyCommand::ccHandler() {
	
}

bool AbstractPowerSupplyCommand::checkState(common::powersupply::PowerSupplyStates state_to_check) {
	CHAOS_ASSERT(powersupply_drv)
	int err = 0;
	int state_id = 0;
	std::string state_str;
	if((err=powersupply_drv->getState(&state_id, state_str, *i_driver_timeout?*i_driver_timeout:10000)) != 0) {
		setWorkState(false);
		std::string error =  boost::str( boost::format("Error getting the powersupply state = %1% ") % err);
		writeErrorMessage(error);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}
	return state_id == state_to_check;
}

void AbstractPowerSupplyCommand::getState(int& current_state, std::string& current_state_str) {
	CHAOS_ASSERT(powersupply_drv)
	int err = 0;
	std::string state_str;
	if((err=powersupply_drv->getState(&current_state, state_str, *i_driver_timeout?*i_driver_timeout:10000)) != 0) {
		setWorkState(false);
		std::string error =  boost::str( boost::format("Error getting the powersupply state = %1% ") % err);
		writeErrorMessage(error);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}

}

void AbstractPowerSupplyCommand::writeErrorMessage(string error_message) {
	writeErrorMessage(error_message.c_str());
}

void AbstractPowerSupplyCommand::writeErrorMessage(const char * error_message) {
	std::strncpy(o_cmd_last_error, error_message, 256);
}

void AbstractPowerSupplyCommand::setWorkState(bool working_flag) {
	*o_dev_state = working_flag;
}