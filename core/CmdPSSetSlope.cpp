/*
 *	CmdPSSetSlope.cpp
 *	!CHAOS
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

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSSetSlope,CMD_PS_SET_SLOPE_ALIAS,
                                                          "Set rising/falling current slope to a given values (A/s)",
                                                          "c217148e-35da-11e5-8324-333c5188d65a")
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_SET_SLOPE_UP, "Rising slope in A/s (optional), it will be used the slope_up INPUT", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_SET_SLOPE_DOWN, "Falling slope in A/s (optional)", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)

BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdPSSetSlope::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSSetSlope::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	int err = 0;
	float asup = 0.f;
	float asdown = 0.f;
	if(data->hasKey(CMD_PS_SET_SLOPE_UP)){
            asup = data->getDoubleValue(CMD_PS_SET_SLOPE_UP);
    } else {
        asup = *getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "slope_up");
    }
    if(data->hasKey(CMD_PS_SET_SLOPE_DOWN)){
        asdown = data->getDoubleValue(CMD_PS_SET_SLOPE_DOWN);
    } else {
        asdown = *getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "slope_down");
    }

	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
		case common::powersupply::POWER_SUPPLY_STATE_ERROR:
		case common::powersupply::POWER_SUPPLY_STATE_UKN:
			//i need to be in operational to exec
			SCLERR_ << boost::str( boost::format("Bas state for set slope comamnd %1%[%2%]") % o_status % *o_status_id);
			break;

		case common::powersupply::POWER_SUPPLY_STATE_OPEN:
		case common::powersupply::POWER_SUPPLY_STATE_ON:
		case common::powersupply::POWER_SUPPLY_STATE_STANDBY:
			SCLAPP_ << "We can start the set slope command";
			break;

		default:
			SCLERR_ << boost::str( boost::format("Unrecognized state %1%[%2%]") % o_status % *o_status_id);
	}


	if((asup > 0) && (asdown > 0)) {
		SCLDBG_ << " set slope with asup=" << asup << " asdown=" << asdown ;
		if((err = powersupply_drv->setCurrentRampSpeed(asup, asdown ))){
			LOG_AND_TROW(SCLERR_, 1, boost::str( boost::format("Error setting the slope %1%[%2%] with error %3%") % o_status % *o_status_id % err));
		}
	}
	BC_END_RUNNIG_PROPERTY
}

void own::CmdPSSetSlope::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
	BC_END_RUNNIG_PROPERTY;
}

bool own::CmdPSSetSlope::timeoutHandler() {
	BC_END_RUNNIG_PROPERTY;
	return false;
}
