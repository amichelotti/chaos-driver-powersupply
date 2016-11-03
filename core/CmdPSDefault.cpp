/*
 *	CmdPSDefault.cpp
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

#include <string.h>
#include "CmdPSDefault.h"
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

#define LOG_HEAD_CmdPSDefault LOG_TAIL(CmdPSDefault)

#define CMDCU_ INFO_LOG(CmdPSDefault) << "[" << getDeviceID() << "] "
#define CMDCUDBG_ DBG_LOG(CmdPSDefault) << "[" << getDeviceID() << "] "
#define CMDCUERR_ ERR_LOG(CmdPSDefault) << "[" << getDeviceID() << "] "

using namespace driver::powersupply;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;

BATCH_COMMAND_OPEN_DESCRIPTION(driver::powersupply::,CmdPSDefault,
                                                          "Default method",
                                                          "4403148a-35df-11e5-b1f7-7f8214ad6212")
BATCH_COMMAND_CLOSE_DESCRIPTION()
CmdPSDefault::CmdPSDefault() {
	powersupply_drv = NULL;
}

CmdPSDefault::~CmdPSDefault() {
	
}

    // return the implemented handler
uint8_t CmdPSDefault::implementedHandler() {
        //add to default hadnler the acquisition one
	return  AbstractPowerSupplyCommand::implementedHandler() |
    HandlerType::HT_Acquisition;
}

    // Start the command execution
void CmdPSDefault::setHandler(c_data::CDataWrapper *data) {
	
	AbstractPowerSupplyCommand::setHandler(data);

	BC_NORMAL_RUNNING_PROPERTY
  }

    // Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void CmdPSDefault::acquireHandler() {
	CMDCU_ << "Acquiring data";
	AbstractPowerSupplyCommand::acquireHandler();
	//force output dataset as changed
	getAttributeCache()->setOutputDomainAsChanged();
}
