/*
 *	CmdPSSetCurrent.cpp
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


#include "CmdPSSetCurrent.h"

#include <cmath>
#include <boost/format.hpp>

#define LOG_HEAD "[CmdPSSetCurrent-" << getUID() << "] "
#define SCLAPP_ LAPP_ << LOG_HEAD
#define SCLDBG_ LDBG_ << LOG_HEAD
#define SCLERR_ LERR_ << LOG_HEAD


namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;


// return the implemented handler
uint8_t own::CmdPSSetCurrent::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSSetCurrent::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	int err = 0;

	float current = 0.f;
	float slope_speed = 0.f;
	
	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
		case common::powersupply::POWER_SUPPLY_STATE_ERROR:
		case common::powersupply::POWER_SUPPLY_STATE_UKN:
			//i need to be in operational to exec
			throw chaos::CException(1, boost::str( boost::format("Bas state for set current comamnd %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
			break;
			
		case common::powersupply::POWER_SUPPLY_STATE_OPEN:
		case common::powersupply::POWER_SUPPLY_STATE_ON:
		case common::powersupply::POWER_SUPPLY_STATE_STANDBY:
			SCLDBG_ << "We can start the set current command";
			break;
			
		default:
			throw chaos::CException(2, boost::str( boost::format("Unrecognized state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
	}
	
	//set comamnd timeout for this instance
	SCLDBG_ << "Checking for timout";
	if(*i_command_timeout) {
		SCLDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
		setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	if(!data || !data->hasKey(CMD_PS_SET_CURRENT)) {
		throw chaos::CException(1, boost::str( boost::format("Set current parameter not present") % o_status % *o_status_id), std::string(__FUNCTION__));
	}
    
    current = static_cast<float>(data->getDoubleValue(CMD_PS_SET_CURRENT));

	if(*o_current_sp > current) {
		SCLDBG_ << "The new current is lower then actual = " << *o_current_sp;
		slope_speed  = *i_slope_down;
	}else {
		SCLDBG_ << "The new current is higher then actual = " << *o_current_sp;
		slope_speed  = *i_slope_up;
	}
	SCLDBG_ << "compute timeout for set current = " << current;
	int32_t delta_current = std::abs(*o_current_sp - current);
	SCLDBG_ << "Delta current is = " << delta_current;
	SCLDBG_ << "Slope speed is = " << slope_speed;
	uint64_t computed_timeout = (std::ceil((delta_current / slope_speed)) * 1000000) + 2000000; //add two seconds for test
	
	//set current set poi into the output channel
	if(*i_setpoint_affinity && (delta_current < *i_setpoint_affinity)) {
		SCLDBG_ << "New current don't pass affinity check affinity_check = " << delta_current << " setpoint_affinity = "<<*i_setpoint_affinity;
	}
	
	SCLDBG_ << "Set current to value" << current;
	if((err = powersupply_drv->setCurrentSP(current)) != 0) {
		throw chaos::CException(2, boost::str( boost::format("Error %1% setting current to %2%") % err % current), std::string(__FUNCTION__));
	}
	
	//assign new current setpoint
	*o_current_sp = current;
	setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, computed_timeout);
	setWorkState(true);
}

void own::CmdPSSetCurrent::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
}

bool own::CmdPSSetCurrent::timeoutHandler() {
	//move the state machine on fault
	SCLDBG_ << "Timeout reached  with readout current " << *o_current;
	setWorkState(false);
	bool result = false;
	if( *i_delta_setpoint && (std::abs(*o_current - *o_current_sp) < *i_delta_setpoint)) {
		std::string error =  "Out of SP";
		SL_FAULT_RUNNIG_STATE
		writeErrorMessage(error);
	}else {
		SL_END_RUNNIG_STATE
	}
	return result;
}