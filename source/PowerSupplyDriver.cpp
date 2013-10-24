/*
 *	SiemensS7TcpDriver.h
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


#define PowerSupplyLAPP_		LAPP_ << "[PowerSupplyDriver] "
#define PowerSupplyLDBG_		LDBG_ << "[PowerSupplyDriver] "
#define PowerSupplyLERR_		LERR_ << "[PowerSupplyDriver] "


//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(PowerSupplyDriver, 1.0.0, driver::PowerSupply::PowerSupplyDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(PowerSupplyDriver, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(driver::PowerSupply::PowerSupplyDriver)
CLOSE_REGISTER_PLUGIN

using namespace driver::PowerSupply;

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR_WITH_NS(driver::PowerSupply, PowerSupplyDriver) {
	
}

//default descrutcor
PowerSupplyDriver::~PowerSupplyDriver() {
	
}

void PowerSupplyDriver::driverInit(const char *initParameter) throw(chaos::CException) {
	PowerSupplyLAPP_ << "Init PowerSupply driver";
}

void PowerSupplyDriver::driverDeinit() throw(chaos::CException) {
	PowerSupplyLAPP_ << "Deinit PowerSupply driver";
}
