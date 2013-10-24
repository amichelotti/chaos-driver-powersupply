/*
 *	PowerSupplyControlUnit.cpp
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

#include "PowerSupplyControlUnit.h"

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace driver::PowerSupply;

#define PowerSupplyCUAPP_ LAPP_ << "[PowerSupplyControlUnit] - "


/*
 Construct a new CU with an identifier
 */
PowerSupplyControlUnit::PowerSupplyControlUnit(string _device_id):device_id(_device_id) {
}

PowerSupplyControlUnit::~PowerSupplyControlUnit() {
	
}

/*
 Return the default configuration
 */
void PowerSupplyControlUnit::unitDefineActionAndDataset() throw(chaos::CException) {
}

void PowerSupplyControlUnit::unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver) {
	chaos::cu::driver_manager::driver::DrvRequestInfo drv1 = {"PowerSupplyDriver", "1.0.0", "parameter" };
	neededDriver.push_back(drv1);
}

// Abstract method for the initialization of the control unit
void PowerSupplyControlUnit::unitInit() throw(chaos::CException) {
	PowerSupplyCUAPP_ "unitInit";
	//plc_s7_accessor = AbstractControlUnit::getAccessoInstanceByIndex(0);
}

// Abstract method for the start of the control unit
void PowerSupplyControlUnit::unitStart() throw(chaos::CException) {
	PowerSupplyCUAPP_ "unitStart";
}


//intervalled scheduled method
void PowerSupplyControlUnit::unitRun() throw(chaos::CException) {
	PowerSupplyCUAPP_ "unitRun";
}

// Abstract method for the stop of the control unit
void PowerSupplyControlUnit::unitStop() throw(chaos::CException) {
	PowerSupplyCUAPP_ "unitStop";
}

// Abstract method for the deinit of the control unit
void PowerSupplyControlUnit::unitDeinit() throw(chaos::CException) {
	PowerSupplyCUAPP_ "unitDeinit";
}
