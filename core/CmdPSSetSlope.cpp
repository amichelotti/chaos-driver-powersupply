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

#define SCLAPP_ INFO_LOG(CmdPSSetSlope) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdPSSetSlope) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdPSSetSlope) << "[" << getDeviceID() << "] "

namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSSetSlope,CMD_PS_SET_SLOPE_ALIAS,
                                                          "Set rising/falling current slope to a given values (A/s)",
                                                          "c217148e-35da-11e5-8324-333c5188d65a")
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_SET_SLOPE_UP, "Rising slope in A/s (optional), it will be used the slope_up INPUT", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_SET_SLOPE_DOWN, "Falling slope in A/s (optional)", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)

BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdPSSetSlope::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}

void own::CmdPSSetSlope::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
        AbstractPowerSupplyCommand::acquireHandler();

	int err = 0;
	float asup = 0.f;
	float asdown = 0.f;
	if(data->hasKey(CMD_PS_SET_SLOPE_UP)){
            asup = data->getDoubleValue(CMD_PS_SET_SLOPE_UP);
    } else {
        asup = *i_asup;
    }
    if(data->hasKey(CMD_PS_SET_SLOPE_DOWN)){
        asdown = data->getDoubleValue(CMD_PS_SET_SLOPE_DOWN);
    } else {
        asdown = *i_asdown;
    }

      if((isnormal(asdown)==false)|| (isnormal(asup)==false)){
        SCLERR_ << "Set slope parameter is not a valid double number (nan?)";
        BC_END_RUNNING_PROPERTY;
        return;
    }
	

	if((asup > 0) && (asdown > 0)) {
		SCLDBG_ << " set slope with asup=" << asup << " asdown=" << asdown ;
		if((err = powersupply_drv->setCurrentRampSpeed(asup, asdown ))){
			SCLERR_<<"error setting slope up:"<<asup<<" slope down:"<<asdown;
		}
	}
        *i_asup=asup;
        *i_asdown=asdown;
        getAttributeCache()->setInputDomainAsChanged();
        pushInputDataset();
	BC_END_RUNNING_PROPERTY
}

void own::CmdPSSetSlope::ccHandler() {
        AbstractPowerSupplyCommand::acquireHandler();
	BC_END_RUNNING_PROPERTY;
}

bool own::CmdPSSetSlope::timeoutHandler() {
	BC_END_RUNNING_PROPERTY;
	return false;
}
