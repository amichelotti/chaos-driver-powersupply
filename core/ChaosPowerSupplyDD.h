/*
 *      Power Supply Device Driver
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
#ifndef __driver_ChaosPowerSupplyDD_h__
#define __driver_ChaosPowerSupplyDD_h__


// include your class/functions headers here

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <common/powersupply/powersupply.h>
//this need to be out the nasmespace
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(ChaosPowerSupplyDD)
namespace cu_driver = chaos::cu::driver_manager::driver;

namespace chaos {
    namespace driver {
        namespace powersupply {
            
            /*
             driver definition
             */
            class ChaosPowerSupplyDD: ADD_CU_DRIVER_PLUGIN_SUPERCLASS,public ::common::powersupply::AbstractPowerSupply{
                
            protected:
                 boost::mutex io_mux;

                ::common::powersupply::AbstractPowerSupply* power;
            public:
                ChaosPowerSupplyDD();
                ~ChaosPowerSupplyDD();
                //! Execute a command
                cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);
                
                void driverDeinit()  throw(chaos::CException);
 /**
             @brief sets the current polarity
             @param pol if >0 sets positive current polarity, if <0 sets negative current polarity, =0 opens the circuit, no current
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
             int setPolarity(int pol,uint32_t timeo_ms=0);
            
            /**
             @brief gets the current polarity
             @param pol returns the polarity if >0 positive current polarity, if <0 negative current polarity, =0 circuit is open, no current
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
            int getPolarity(int* pol,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            /**
             @brief sets the current set point
             @param current the current set point to reach expressed in ampere
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
            
             int setCurrentSP(float current,uint32_t timeo_ms=0);
            
            
            /**
             @brief gets the actual current set point
             @param current returns the current readout expressed in ampere
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
            
             int getCurrentSP(float* current,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            /**
             @brief start ramp toward the predefined current set point
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success (current readout equal to set) or an error code
             */
            
             int startCurrentRamp(uint32_t timeo_ms=0);
            
            /**
             @brief gets the voltage output
             @param volt gets the voltage readout expressed in volt
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
            
             int getVoltageOutput(float* volt,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            
            /**
             @brief gets the current output
             @param current returns the current readout expressed in ampere
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
            
             int getCurrentOutput(float* current,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            
            /**
             @brief set the current rising/falling ramp speed
             @param asup rising ramp speed expressed in ampere/second
             @param asdown rising ramp speed expressed in ampere/second
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
             int setCurrentRampSpeed(float asup,float asdown,uint32_t timeo_ms=0);
            
            
            /**
             @brief resets alarms
             @param alrm a 64 bit field containing the alarms to be reset (-1 all alarms)
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
             int resetAlarms(uint64_t alrm,uint32_t timeo_ms=0);
            
            /**
             @brief get alarms
             @param alrm returns a 64 bit field PowerSupplyEvents containing the alarms
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
             int getAlarms(uint64_t*alrm,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            
            /**
             @brief shuts down the power supply, the communication could drop
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
             int shutdown(uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            
            /**
             @brief standby the power supply
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
             int standby(uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            /**
             @brief poweron the power supply after a standby or shutdown (if possible)
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success
             */
             int poweron(uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            
            /**
             @brief gets the power supply state
             @param state returns a bit field of PowerSupplyStates
             @param desc return a string description
             @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
             @return 0 if success or an error code
             */
             int getState(int* state,std::string& desc,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            
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
             int getSWVersion(std::string& version,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
            /**
             @brief returns the HW version of the powersupply
             @param version returning string
             @return 0 if success or an error code
             
             */
             int getHWVersion(std::string& version,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
            
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
             int getMaxMinCurrent(float*max,float*min);
            
            /**
             @brief returns the max min voltage of the power suppy
             @param max returns the max voltage that the power supply can output
             @param min returns the min voltage that the power supply can output
             @return 0 if success or an error code
             
             */
             int getMaxMinVoltage(float*max,float*min);
            /**
             @brief returns the bitfield of implemented alarms
             @param alarm 64bit bitfield containing the implemented alarms
             @return 0 if success or an error code
             
             */
             int getAlarmDesc(uint64_t* alarm);
            
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
             uint64_t getFeatures() ;
            };
        }
    }
}

#endif
