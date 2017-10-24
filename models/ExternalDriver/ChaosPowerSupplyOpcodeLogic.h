/*
 *	ChaosPowerSupplyOpcodeLogic.h
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

#ifndef __ExampleControlUnit__DAB3EB0_DEBD_47DC_8DB8_05E1E8BB501E_ChaosPowerSupplyOpcodeLogic_h
#define __ExampleControlUnit__DAB3EB0_DEBD_47DC_8DB8_05E1E8BB501E_ChaosPowerSupplyOpcodeLogic_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/cu_toolkit/driver_manager/driver/OpcodeExternalCommandMapper.h>

#define POWER_SUPPLY_DEFAULT_TIMEOUT 5000

namespace chaos {
    namespace driver {
        namespace powersupply {
            class ChaosPowerSupplyOpcodeLogic:
            public chaos::cu::driver_manager::driver::OpcodeExternalCommandMapper {
                chaos::common::data::CDWUniquePtr powersupply_init_pack;
            protected:
                int sendInit(cu::driver_manager::driver::DrvMsgPtr cmd);
                
                int sendDeinit(cu::driver_manager::driver::DrvMsgPtr cmd);
                /**
                 @brief sets the current polarity
                 @param pol if >0 sets positive current polarity, if <0 sets negative current polarity, =0 opens the circuit, no current
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                int setPolarity(cu::driver_manager::driver::DrvMsgPtr cmd, int pol,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief gets the current polarity
                 @param pol returns the polarity if >0 positive current polarity, if <0 negative current polarity, =0 circuit is open, no current
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                int getPolarity(cu::driver_manager::driver::DrvMsgPtr cmd, int* pol,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief sets the current set point
                 @param current the current set point to reach expressed in ampere
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                
                int setCurrentSP(cu::driver_manager::driver::DrvMsgPtr cmd, float current,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief gets the actual current set point
                 @param current returns the current readout expressed in ampere
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                
                int getCurrentSP(cu::driver_manager::driver::DrvMsgPtr cmd, float* current,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief start ramp toward the predefined current set point
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success (current readout equal to set) or an error code
                 */
                
                int startCurrentRamp(cu::driver_manager::driver::DrvMsgPtr cmd, uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief gets the voltage output
                 @param volt gets the voltage readout expressed in volt
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                
                int getVoltageOutput(cu::driver_manager::driver::DrvMsgPtr cmd, float* volt,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief gets the current output
                 @param current returns the current readout expressed in ampere
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                
                int getCurrentOutput(cu::driver_manager::driver::DrvMsgPtr cmd, float* current,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief set the current rising/falling ramp speed
                 @param asup rising ramp speed expressed in ampere/second
                 @param asdown rising ramp speed expressed in ampere/second
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                int setCurrentRampSpeed(cu::driver_manager::driver::DrvMsgPtr cmd, float asup,float asdown,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief resets alarms
                 @param alrm a 64 bit field containing the alarms to be reset (-1 all alarms)
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success
                 */
                int resetAlarms(cu::driver_manager::driver::DrvMsgPtr cmd, uint64_t alrm,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief get alarms
                 @param alrm returns a 64 bit field PowerSupplyEvents containing the alarms
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success
                 */
                int getAlarms(cu::driver_manager::driver::DrvMsgPtr cmd, uint64_t*alrm,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief shuts down the power supply, the communication could drop
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success
                 */
                int shutdown(cu::driver_manager::driver::DrvMsgPtr cmd, uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief standby the power supply
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success
                 */
                int standby(cu::driver_manager::driver::DrvMsgPtr cmd, uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                /**
                 @brief poweron the power supply after a standby or shutdown (if possible)
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success
                 */
                int poweron(cu::driver_manager::driver::DrvMsgPtr cmd, uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief gets the power supply state
                 @param state returns a bit field of PowerSupplyStates
                 @param desc return a string description
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                int getState(cu::driver_manager::driver::DrvMsgPtr cmd, int* state,std::string& desc,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                
                /**
                 @brief returns the SW/FW version of the driver/FW
                 @param version returning string
                 @return 0 if success or an error code
                 */
                int getSWVersion(cu::driver_manager::driver::DrvMsgPtr cmd, std::string& version,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief returns the HW version of the powersupply
                 @param version returning string
                 @return 0 if success or an error code
                 
                 */
                int getHWVersion(cu::driver_manager::driver::DrvMsgPtr cmd, std::string& version,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief returns the current sensibility of the power supply
                 @param sens sensibility in ampere
                 @return 0 if success or an error code
                 
                 */
                int getCurrentSensibility(cu::driver_manager::driver::DrvMsgPtr cmd, float *sens);
                /**
                 @brief returns the voltage sensibility of the power supply
                 @param sens sensibility in volt
                 @return 0 if success or an error code
                 */
                int getVoltageSensibility(cu::driver_manager::driver::DrvMsgPtr cmd, float *sens);
                
                
                /**
                 @brief set the current sensibility of the power supply
                 @param sens sensibility in ampere
                 @return 0 if success or an error code
                 
                 */
                int setCurrentSensibility(cu::driver_manager::driver::DrvMsgPtr cmd, float sens);
                
                /**
                 @brief set the voltage sensibility of the power supply
                 @param sens sensibility in volt
                 @return 0 if success or an error code
                 */
                int setVoltageSensibility(cu::driver_manager::driver::DrvMsgPtr cmd, float sens);
                /**
                 @brief returns the max min current of the power suppy
                 @param max returns the max current that the power supply can output
                 @param min returns the min current that the power supply can output
                 @return 0 if success or an error code
                 
                 */
                int getMaxMinCurrent(cu::driver_manager::driver::DrvMsgPtr cmd, float*max,float*min);
                
                /**
                 @brief returns the max min voltage of the power suppy
                 @param max returns the max voltage that the power supply can output
                 @param min returns the min voltage that the power supply can output
                 @return 0 if success or an error code
                 
                 */
                int getMaxMinVoltage(cu::driver_manager::driver::DrvMsgPtr cmd, float*max,float*min);
                /**
                 @brief returns the bitfield of implemented alarms
                 @param alarm 64bit bitfield containing the implemented alarms
                 @return 0 if success or an error code
                 
                 */
                int getAlarmDesc(cu::driver_manager::driver::DrvMsgPtr cmd, uint64_t* alarm);
                
                /**
                 @brief force max current, it is used to calculated max current of the powersupply
                 @param max the max current that the power supply can output
                 @return 0 if success or an error code
                 */
                int forceMaxCurrent(cu::driver_manager::driver::DrvMsgPtr cmd, float max);
                
                /**
                 @brief force max voltage, it is used to calculated max current of the powersupply
                 @param max the max voltage that the power supply can output
                 @return 0 if success or an error code
                 */
                int forceMaxVoltage(cu::driver_manager::driver::DrvMsgPtr cmd, float max);
                
                uint64_t getFeatures(cu::driver_manager::driver::DrvMsgPtr cmd);
            public:
                ChaosPowerSupplyOpcodeLogic(chaos::cu::driver_manager::driver::RemoteIODriverProtocol *_remote_driver);
                ~ChaosPowerSupplyOpcodeLogic();
                void driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException);
                void driverDeinit() throw(chaos::CException);
                chaos::cu::driver_manager::driver::MsgManagmentResultType::MsgManagmentResult execOpcode(chaos::cu::driver_manager::driver::DrvMsgPtr cmd);
                int asyncMessageReceived(chaos::common::data::CDWUniquePtr message);
            };
        }
    }
}
#endif /* __ExampleControlUnit__DAB3EB0_DEBD_47DC_8DB8_05E1E8BB501E_SinGeneratoOpcodeLogic_h */
