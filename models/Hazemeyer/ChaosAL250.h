/*
 *      Hazemeyer Device Driver
 *	!CHAOS
 *	Created by Alessandro D'Uffizi
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
#ifndef __driver_AL250_h__
#define __driver_AL250_h__


// include your class/functions headers here

#include "../../../../common/powersupply/models/Hazemeyer/AL250PowerSupply.h"
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <driver/powersupply/core/ChaosPowerSupplyDD.h>
//this need to be out the namespace
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(C_AL250)
namespace cu_driver = chaos::cu::driver_manager::driver;

namespace chaos {
    namespace driver {
        namespace powersupply {
            
            /*
             driver definition
             */
            class C_AL250: public ChaosPowerSupplyDD{
                //::common::powersupply::AL250 *Unit;
                void driverInit(const char *initParameter) throw(chaos::CException);
                void driverInit(const chaos::common::data::CDataWrapper& json) throw(chaos::CException);
            public:
                C_AL250();
                ~C_AL250();
            };
        }
    }
}

#endif
