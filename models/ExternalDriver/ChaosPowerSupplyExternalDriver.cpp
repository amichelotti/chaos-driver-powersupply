/*
 *	ChaosPowerSupplyExternalDriver.cpp
 *
 *	!CHAOS
 *	Created by bisegni.
 *
 *    	Copyright 25/07/2017 INFN, National Institute of Nuclear Physics
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

#include "ChaosPowerSupplyExternalDriver.h"

using namespace chaos::driver::powersupply;
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(ChaosPowerSupplyRemoteServerDriver, 1.0.0, chaos::driver::powersupply::ChaosPowerSupplyRemoteServerDriver)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(ChaosPowerSupplyRemoteClientDriver, 1.0.0, chaos::driver::powersupply::ChaosPowerSupplyRemoteClientDriver)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION
