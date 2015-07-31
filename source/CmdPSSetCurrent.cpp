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
#include <boost/lexical_cast.hpp>

#define LOG_HEAD_CmdPSSetCurrent LOG_TAIL(CmdPSSetCurrent)
#define SCLAPP_ LAPP_ << LOG_HEAD_CmdPSSetCurrent
#define SCLDBG_ LDBG_ << LOG_HEAD_CmdPSSetCurrent
#define SCLERR_ LERR_ << LOG_HEAD_CmdPSSetCurrent


namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSSetCurrent,CMD_PS_SET_SLOPE_ALIAS,
                                                          "Set current (A) to the given value",
                                                          "72882f3e-35da-11e5-985f-334fcd6dff22")
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_SET_CURRENT, "current in A",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()

// return the implemented handler
uint8_t own::CmdPSSetCurrent::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSSetCurrent::setHandler(c_data::CDataWrapper *data) {
    chaos::common::data::RangeValueInfo current_sp_attr_info;
    chaos::common::data::RangeValueInfo attributeInfo;
	AbstractPowerSupplyCommand::setHandler(data);
        const double*max_current;
	int err = 0;
	o_current = getAttributeCache()->getROPtr<double>(DOMAIN_OUTPUT, "current");
	o_current_sp = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "current_sp");
	
	i_slope_up = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "slope_up");
	i_slope_down = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "slope_down");
	i_command_timeout = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "command_timeout");
	i_delta_setpoint = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "delta_setpoint");
	i_setpoint_affinity = getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "setpoint_affinity");
	max_current =  getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "max_current");
	float current = 0.f;
	float slope_speed = 0.f;
	
	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
		case common::powersupply::POWER_SUPPLY_STATE_ERROR:
		case common::powersupply::POWER_SUPPLY_STATE_UKN:
			//i need to be in operational to exec
			TROW_ERROR(1, boost::str( boost::format("Bad state for set current comamnd %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
			break;
			
		case common::powersupply::POWER_SUPPLY_STATE_OPEN:
		case common::powersupply::POWER_SUPPLY_STATE_ON:
		case common::powersupply::POWER_SUPPLY_STATE_STANDBY:
			SCLDBG_ << "We can start the set current command";
			break;
			
		default:
			TROW_ERROR(2, boost::str( boost::format("Unrecognized state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
	}
	
	//set comamnd timeout for this instance
	SCLDBG_ << "Checking for timout";
	if(*i_command_timeout) {
		SCLDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	if(!data || !data->hasKey(CMD_PS_SET_CURRENT)) {
		TROW_ERROR(3, boost::str( boost::format("Set current parameter not present") % o_status % *o_status_id), std::string(__FUNCTION__))
	}
    
    current = static_cast<float>(data->getDoubleValue(CMD_PS_SET_CURRENT));
    if(max_current && (current>*max_current)){
        TROW_ERROR(10, boost::str( boost::format("current %1 > max current %2 %1%[%2%]") % current % max_current), std::string(__FUNCTION__))

    }
    SCLDBG_ << "compute timeout for set current = " << current;
    
	if(*o_current_sp > current) {
		SCLDBG_ << "The new current is lower then actual = " << *o_current_sp << "[new "<<current<<"]";
		slope_speed  = *i_slope_down;
	}else {
		SCLDBG_ << "The new current is higher then actual = " << *o_current_sp << "[new "<<current<<"]";;
		slope_speed  = *i_slope_up;
	}
	
	//compute the delta for check if we are on the rigth current at the end of the job
	SCLDBG_ << "Delta current is = " << *i_delta_setpoint;
	SCLDBG_ << "Slope speed is = " << slope_speed;
	uint64_t computed_timeout = (std::ceil((std::abs(*o_current_sp - current) / slope_speed)) * 1000);
	computed_timeout = computed_timeout * 1.2; //add 20% to the real timeout
    
	//set current set poi into the output channel
	if(*i_setpoint_affinity && (*i_delta_setpoint < *i_setpoint_affinity)) {
		SCLDBG_ << "New current don't pass affinity check affinity_check = " << *i_delta_setpoint << " setpoint_affinity = "<<*i_setpoint_affinity;
	}
	
	SCLDBG_ << "Set current to value " << current;
	SCLDBG_ << "computed_timeout is = " << computed_timeout;
	if((err = powersupply_drv->setCurrentSP(current)) != 0) {
		TROW_ERROR(5, boost::str(boost::format("Error %1% setting current to %2%") % err % current), std::string(__FUNCTION__))
	}
	if((err = powersupply_drv->startCurrentRamp()) != 0) {
		TROW_ERROR(6, boost::str(boost::format("Error %1% setting current to %2%") % err % current), std::string(__FUNCTION__))
	}
	//assign new current setpoint
	*o_current_sp = current;
	powersupply_drv->accessor->base_opcode_priority=100;
	setWorkState(true);
	//set runnign  property to exsculisve untile command has finisced
	BC_EXEC_RUNNIG_PROPERTY
	setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, /*(computed_timeout==0)?std::numeric_limits<int>::max():*/computed_timeout);
}

void own::CmdPSSetCurrent::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
}

bool own::CmdPSSetCurrent::timeoutHandler() {
	//move the state machine on fault
	SCLDBG_ << "Timeout reached  with readout current " << *o_current;
	setWorkState(false);
	powersupply_drv->accessor->base_opcode_priority=50;
	bool result = false;
	if( *i_delta_setpoint && (std::abs(*o_current - *o_current_sp) > *i_delta_setpoint)) {
		std::string error =  "Out of SP";
		BC_FAULT_RUNNIG_PROPERTY
	}else {
		BC_END_RUNNIG_PROPERTY
	}
	return result;
}
