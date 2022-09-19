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
#include <driver/data-import/models/dante/DanteDriver.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include "../../core/ChaosPowerSupplyDD.h"
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
                ::driver::data_import::DanteDriver dante;
                double current;
                std::string protocol2String(int32_t t);
                int alarmType,polarityType,interfaceType,protocol;
                //void driverDeinit() throw(chaos::CException);
            public:
                DanteDD();
                ~DanteDD();
                chaos::common::data::CDWUniquePtr getDrvProperties();
                  
                //int setDrvProperty(const std::string& key, const std::string& value);
                 bool isBypass();

    /**
             @brief sets the current polarity
             @param pol if >0 sets positive current polarity, if <0 sets negative current polarity, =0 opens the circuit, no current
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
    int setPolarity(int pol, uint32_t timeo_ms = 0);

    /**
             @brief gets the current polarity
             @param pol returns the polarity if >0 positive current polarity, if <0 negative current polarity, =0 circuit is open, no current
             @param polsp returns the polarity if >0 positive current polarity, if <0 negative current polarity, =0 circuit is open, no current

             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
    int getPolarity(int *pol, int*polsp=NULL,uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief sets the current set point
             @param current the current set point to reach expressed in ampere
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */

    int setCurrentSP(float current, uint32_t timeo_ms = 0);

    /**
             @brief gets the actual current set point
             @param current returns the current readout expressed in ampere
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */

    int getCurrentSP(float *current, uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief start ramp toward the predefined current set point
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success (current readout equal to set) or an error code
             */

    int startCurrentRamp(uint32_t timeo_ms = 0);

    /**
             @brief gets the voltage output
             @param volt gets the voltage readout expressed in volt
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */

    int getVoltageOutput(float *volt, uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief gets the current output
             @param current returns the current readout expressed in ampere
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */

    int getCurrentOutput(float *current, uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief set the current rising/falling ramp speed
             @param asup rising ramp speed expressed in ampere/second
             @param asdown rising ramp speed expressed in ampere/second
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
    int setCurrentRampSpeed(float asup, float asdown, uint32_t timeo_ms = 0);

    /**
             @brief resets alarms
             @param alrm a 64 bit field containing the alarms to be reset (-1 all alarms)
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
    int resetAlarms(uint64_t alrm, uint32_t timeo_ms = 0);

    /**
             @brief get alarms
             @param alrm returns a 64 bit field PowerSupplyEvents containing the alarms
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
    int getAlarms(uint64_t *alrm, uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief shuts down the power supply, the communication could drop
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
    int shutdown(uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief standby the power supply
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
    int standby(uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);
    /**
             @brief poweron the power supply after a standby or shutdown (if possible)
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
    int poweron(uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief gets the power supply state
             @param state returns a bit field of PowerSupplyStates
             @param statesp returns a bit field of PowerSupplyStates

             @param desc return a string description
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
    int getState(int *state, std::string &desc, int*statesp=NULL,uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief initialize and poweron the power supply
             @return 0 if success
             */
    int initPS();

    /**
             @brief de-initialize the power supply and close the communication
             @return 0 if success
             */
    int deinitPS();

    /**
             @brief returns the SW/FW version of the driver/FW
             @param version returning string
             @return 0 if success or an error code
             */
    int getSWVersion(std::string &version, uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief returns the HW version of the powersupply
             @param version returning string
             @return 0 if success or an error code
             
             */
    int getHWVersion(std::string &version, uint32_t timeo_ms = POWER_SUPPLY_DEFAULT_TIMEOUT);

    /**
             @brief returns the current sensibility of the power supply
             @param sens sensibility in ampere
             @return 0 if success or an error code
             
             */
    int getCurrentSensibility(float *sens);
    /**
             @brief returns the voltage sensibility of the power supply
             @param sens sensibility in volt
             @return 0 if success or an error code
             */
    int getVoltageSensibility(float *sens);

    /**
             @brief set the current sensibility of the power supply
             @param sens sensibility in ampere
             @return 0 if success or an error code
             
             */
    int setCurrentSensibility(float sens);

    /**
             @brief set the voltage sensibility of the power supply
             @param sens sensibility in volt
             @return 0 if success or an error code
             */
    int setVoltageSensibility(float sens);
    /**
             @brief returns the max min current of the power suppy
             @param max returns the max current that the power supply can output
             @param min returns the min current that the power supply can output
             @return 0 if success or an error code
             
             */
    int getMaxMinCurrent(float *max, float *min);

    /**
             @brief returns the max min voltage of the power suppy
             @param max returns the max voltage that the power supply can output
             @param min returns the min voltage that the power supply can output
             @return 0 if success or an error code
             
             */
    int getMaxMinVoltage(float *max, float *min);
    /**
             @brief returns the bitfield of implemented alarms
             @param alarm 64bit bitfield containing the implemented alarms
             @return 0 if success or an error code
             
             */
    int getAlarmDesc(uint64_t *alarm);

    /**
             @brief force max current, it is used to calculated max current of the powersupply
             @param max the max current that the power supply can output
             @return 0 if success or an error code
             */
    int forceMaxCurrent(float max);

    /**
             @brief force max voltage, it is used to calculated max current of the powersupply
             @param max the max voltage that the power supply can output
             @return 0 if success or an error code
             */
    int forceMaxVoltage(float max);
    uint64_t getFeatures();

            };
        }
    }
}

#endif
