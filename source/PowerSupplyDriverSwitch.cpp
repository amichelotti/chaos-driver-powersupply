/*
 *	SiemensS7DriverSwitch.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
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

#include "PowerSupplyDriver.h"

#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>


using namespace driver::PowerSupply;


//! Execute a command
chaos::cu::driver_manager::driver::MsgManagmentResultType::MsgManagmentResult PowerSupplyDriver::execOpcode(chaos::cu::driver_manager::driver::DrvMsgPtr cmd) {
	chaos::cu::driver_manager::driver::MsgManagmentResultType::MsgManagmentResult result = chaos::cu::driver_manager::driver::MsgManagmentResultType::MMR_EXECUTED;
	
	return result;
}
