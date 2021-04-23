/*
 *     Dante Device Driver
 * 
 * This driver interface Dante Control systems to control PS
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2020 INFN, National Institute of Nuclear Physics
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


/*
configuration example:
   "cudk_driver_description": [
                            {
                                "cudk_driver_description_name": "DanteDD",
                                "cudk_driver_description_version": "1.0.0",
                                "cudk_driver_description_init_parameter": "{\"server_url\":[\"192.168.192.107:11211\"],
                                \"data_keys\":[\"QUATT007_STA\",\"QUATT007_DYN\"],
                                \"data_pack_len\":1024,\"dante_server_url\":\"http://danteweb.lnf.infn.it:8000\",\"dante_element\":\"QUATT007\",\"dyn_ds\":{\"keybind\":\"QUATT007_DYN\",\"dataset\":[{\"name\":\"elemName\",\"type\":\"string\",\"description\":\"elementName\",\"offset\":0,\"len\":8,\"lbe\":false},{\"name\":\"status\",\"type\":\"int32\",\"description\":\"status\",\"offset\":8,\"len\":4,\"lbe\":true},{\"name\":\"consoleName\",\"type\":\"int32\",\"description\":\"Console name\",\"offset\":12,\"len\":4,\"lbe\":true},{\"name\":\"errorMask\",\"type\":\"int32\",\"description\":\"Error mask\",\"offset\":16,\"len\":4,\"lbe\":true},{\"name\":\"cmdExeStartTome\",\"type\":\"int32\",\"description\":\"Command Execution Start time\",\"offset\":20,\"len\":4,\"lbe\":true},{\"name\":\"maxCmdExeTime\",\"type\":\"int32\",\"description\":\"Command Max Execution Time\",\"offset\":24,\"len\":4,\"lbe\":true},{\"name\":\"cmdExecution\",\"type\":\"int32\",\"description\":\"Command Execution\",\"offset\":28,\"len\":4,\"lbe\":true},{\"name\":\"onLine\",\"type\":\"bool\",\"description\":\"is Online\",\"offset\":32,\"len\":1,\"lbe\":true},{\"name\":\"byPass\",\"type\":\"bool\",\"description\":\"is bypass\",\"offset\":33,\"len\":1,\"lbe\":true},{\"name\":\"remote\",\"type\":\"bool\",\"description\":\"is remote\",\"offset\":34,\"len\":1,\"lbe\":true},{\"name\":\"busy\",\"type\":\"bool\",\"description\":\"is busy\",\"offset\":35,\"len\":1,\"lbe\":true},{\"name\":\"triggerArmed\",\"type\":\"bool\",\"description\":\"is \",\"offset\":36,\"len\":1,\"lbe\":true},{\"name\":\"rampOn\",\"type\":\"bool\",\"description\":\"is \",\"offset\":37,\"len\":1,\"lbe\":true},{\"name\":\"currentPreSetting\",\"type\":\"double\",\"description\":\"bo\",\"offset\":38,\"len\":8,\"lbe\":true},{\"name\":\"statusSettting\",\"type\":\"int32\",\"description\":\"status\",\"offset\":46,\"len\":4,\"lbe\":true},{\"name\":\"polaritySettting\",\"type\":\"int32\",\"description\":\"polarity\",\"offset\":50,\"len\":4,\"lbe\":true},{\"name\":\"currentSetting\",\"type\":\"double\",\"description\":\"current setting\",\"offset\":54,\"len\":8,\"lbe\":true},{\"name\":\"slewRateSetting\",\"type\":\"double\",\"description\":\"slew rate setting\",\"offset\":62,\"len\":8,\"lbe\":true},{\"name\":\"cycleMode\",\"type\":\"int32\",\"description\":\"Cycle mode\",\"offset\":70,\"len\":4,\"lbe\":true},{\"name\":\"outputPolarity\",\"type\":\"int32\",\"description\":\"output polarity\",\"offset\":74,\"len\":4,\"lbe\":true},{\"name\":\"outputCurrent\",\"type\":\"double\",\"description\":\"output current\",\"offset\":78,\"len\":8,\"lbe\":true},{\"name\":\"slewRateReadout\",\"type\":\"double\",\"description\":\"slew rate readout\",\"offset\":86,\"len\":8,\"lbe\":true},{\"name\":\"outputVolt\",\"type\":\"double\",\"description\":\"output volt\",\"offset\":94,\"len\":8,\"lbe\":true},{\"name\":\"alarms\",\"type\":\"int64\",\"description\":\"alarms\",\"offset\":102,\"len\":8,\"lbe\":true}]},\"sta_ds\":{\"keybind\":\"QUATT007_STA\",\"dataset\":[{\"name\":\"recordClass\",\"type\":\"int32\",\"description\":\"record Class\",\"offset\":0,\"len\":4,\"lbe\":true},{\"name\":\"elemName\",\"type\":\"string\",\"description\":\"elementName\",\"offset\":4,\"len\":12,\"lbe\":false},{\"name\":\"elemType\",\"type\":\"int32\",\"description\":\"Element Type\",\"offset\":0,\"len\":4,\"lbe\":true}]}}"
                            }

*/
#ifndef __driver_DanteDD_h__
#define __driver_DanteDD_h__
#undef POWER_SUPPLY_DEFAULT_TIMEOUT
#define POWER_SUPPLY_DEFAULT_TIMEOUT 5000

// include your class/functions headers here

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <driver/powersupply/core/ChaosPowerSupplyDD.h>
//this need to be out the namespace
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(DanteDD)
namespace cu_driver = chaos::cu::driver_manager::driver;

namespace chaos {
    namespace driver {
        namespace powersupply {
            
            /*
             driver definition
             */
            class DanteDD: public ChaosPowerSupplyDD{
                
                void driverInit(const char *initParameter) throw(chaos::CException);
                void driverInit(const chaos::common::data::CDataWrapper& json) throw(chaos::CException);

                void driverDeinit() throw(chaos::CException);
            public:
                DanteDD();
                ~DanteDD();
                chaos::common::data::CDWUniquePtr getDrvProperties();
                  
                int setDrvProperty(const std::string& key, const std::string& value);

            };
        }
    }
}

#endif
