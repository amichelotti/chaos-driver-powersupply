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
#define LOG_HEAD_AbstractPowerSupplyCommand LOG_TAIL(AbstractPowerSupplyCommand)
#define CMDCUDBG_ LDBG_ << LOG_HEAD_AbstractPowerSupplyCommand
#include "AbstractPowerSupplyCommand.h"
#include <boost/format.hpp>

using namespace driver::powersupply;
namespace chaos_batch = chaos::common::batch_command;

void AbstractPowerSupplyCommand::setHandler(c_data::CDataWrapper *data) {
	CMDCUDBG_ << "loading pointer for output channel";
	
	o_status_id = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "status_id");
	o_status = getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "status");

	
	//get pointer to the output datase variable
	chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = driverAccessorsErogator->getAccessoInstanceByIndex(0);
	if(power_supply_accessor != NULL) {
		powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
	}
}

// return the implemented handler
uint8_t AbstractPowerSupplyCommand::implementedHandler() {
	return  chaos_batch::HandlerType::HT_Set |
			chaos_batch::HandlerType::HT_Correlation;
}

void AbstractPowerSupplyCommand::ccHandler() {
	
}

bool AbstractPowerSupplyCommand::checkState(common::powersupply::PowerSupplyStates state_to_check) {
	CHAOS_ASSERT(powersupply_drv)
	int err = 0;
	int state_id = 0;
	std::string state_str;
	int32_t i_driver_timeout = getAttributeCache()->getValue<int32_t>(DOMAIN_INPUT, "driver_timeout");
	if((err=powersupply_drv->getState(&state_id, state_str, i_driver_timeout?i_driver_timeout:10000)) != 0) {
		setWorkState(false);
		std::string error =  boost::str( boost::format("Error getting the powersupply state = %1% ") % err);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}
	return state_id == state_to_check;
}

void AbstractPowerSupplyCommand::getState(int& current_state, std::string& current_state_str) {
	CHAOS_ASSERT(powersupply_drv)
	int err = 0;
	std::string state_str;
	int32_t i_driver_timeout = getAttributeCache()->getValue<int32_t>(DOMAIN_INPUT, "driver_timeout");
	if((err=powersupply_drv->getState(&current_state, state_str, i_driver_timeout?i_driver_timeout:10000)) != 0) {
		setWorkState(false);
		std::string error =  boost::str( boost::format("Error getting the powersupply state = %1% ") % err);
		throw chaos::CException(1, error.c_str(), __FUNCTION__);
	}

}

void AbstractPowerSupplyCommand::setWorkState(bool working_flag) {
	int64_t *o_dev_state = getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT, "dev_state");
	*o_dev_state = working_flag;
	//getAttributeCache()->setOutputAttributeValue("cmd_last_error", &working_flag, (uint32_t)strlen(error_message));
}