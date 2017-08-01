/*
 *      Ocem Device Driver
 *	!CHAOS
 *	Created by Andrea Michelotti
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
#ifndef __driver_OcemDD_h__
#define __driver_OcemDD_h__
#undef POWER_SUPPLY_DEFAULT_TIMEOUT
#define POWER_SUPPLY_DEFAULT_TIMEOUT 5000

// include your class/functions headers here

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <driver/powersupply/core/ChaosPowerSupplyDD.h>
//this need to be out the namespace
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(OcemDD)
namespace cu_driver = chaos::cu::driver_manager::driver;

namespace chaos {
    namespace driver {
        namespace powersupply {
            
            /*
             driver definition
             */
            class OcemDD: public ChaosPowerSupplyDD{
                
                void driverInit(const char *initParameter) throw(chaos::CException);
                
            public:
                OcemDD();
                ~OcemDD();
            };
        }
    }
}

#endif
