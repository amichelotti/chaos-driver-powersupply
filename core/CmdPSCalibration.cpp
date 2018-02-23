/*
 *	CmdPSCalibration.cpp
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


#include "CmdPSCalibration.h"

#include <boost/format.hpp>
#include <common/debug/core/debug.h>
#define SCLAPP_ INFO_LOG(CmdPSCalibration) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdPSCalibration) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdPSCalibration) << "[" << getDeviceID() << "] "

namespace own =  driver::powersupply;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace c_data = chaos::common::data;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSCalibration,CMD_PS_CALIBRATE,
                                                          "Perform a calibration cycle",
                                                          "c217148e-35da-11e5-8324-333c5288d65a")
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_CALIBRATE_FROM, "Starting current (A)", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_CALIBRATE_TO, "Ending current (A)", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_CALIBRATE_STEPS, "Steps (A)", chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)


BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdPSCalibration::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}

void own::CmdPSCalibration::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	int err = 0;
	
	if(data->hasKey(CMD_PS_CALIBRATE_FROM)){
            from = data->getDoubleValue(CMD_PS_CALIBRATE_FROM);
        } else{
            DERR("no FROM parameter given");
            return;
        }
    if(data->hasKey(CMD_PS_CALIBRATE_TO)){
        to = data->getDoubleValue(CMD_PS_CALIBRATE_TO);

    } else {
            DERR("no FROM parameter given");
            return;
    }

        if(data->hasKey(CMD_PS_CALIBRATE_STEPS)){
            step = data->getDoubleValue(CMD_PS_CALIBRATE_STEPS);
    } else {
            DERR("no STEP parameter given");
            return;
    }
                DPRINT("performing calibration from %f A to %f A step %f A",from,to,step);
	

}

void own::CmdPSCalibration::ccHandler() {
	AbstractPowerSupplyCommand::ccHandler();
	BC_END_RUNNING_PROPERTY;
}

bool own::CmdPSCalibration::timeoutHandler() {
	BC_END_RUNNING_PROPERTY;
	return false;
}
