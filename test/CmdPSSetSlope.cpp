/*
 *	CmdPSSetSlope.cpp
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


#include "CmdPSSetSlope.h"

#include <boost/format.hpp>

#define LOG_HEAD_CmdPSSetSlope LOG_TAIL(CmdPSSetSlope)

#define SCLAPP_ LAPP_ << LOG_HEAD_CmdPSSetSlope
#define SCLDBG_ LDBG_ << LOG_HEAD_CmdPSSetSlope
#define SCLERR_ LERR_ << LOG_HEAD_CmdPSSetSlope


namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;


// return the implemented handler
uint8_t own::CmdPSSetSlope::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSSetSlope::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	float asup = 0.f;
	float asdown = 0.f;
	
	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
		case common::powersupply::POWER_SUPPLY_STATE_ERROR:
		case common::powersupply::POWER_SUPPLY_STATE_UKN:
			//i need to be in operational to exec
			TROW_ERROR(1, boost::str( boost::format("Bas state for set slope comamnd %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
			break;
			
		case common::powersupply::POWER_SUPPLY_STATE_OPEN:
		case common::powersupply::POWER_SUPPLY_STATE_ON:
		case common::powersupply::POWER_SUPPLY_STATE_STANDBY:
			SCLDBG_ << "We can start the set slope command";
			break;
			
		default:
			TROW_ERROR(1, boost::str( boost::format("Unrecognized state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
	}
	
	
	//set comamnd timeout for this instance
//	SCLDBG_ << "Checking for timout";
//	if(*i_command_timeout) {
//		SCLDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
//		setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
//	}
	
	
	if((asup > 0) && (asdown > 0)) {
		SCLDBG_ << " set slope with asup=" << *i_slope_up << " asdown=" << *i_slope_down ;
		if(powersupply_drv->setCurrentRampSpeed(*i_slope_up, *i_slope_down ) != 0) {
			TROW_ERROR(2, boost::str( boost::format("Error setting the slope %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__))
		}
	}
	
}

void own::CmdPSSetSlope::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
}

bool own::CmdPSSetSlope::timeoutHandler() {
	return true;
}