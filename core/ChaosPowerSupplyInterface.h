//
//  ChaosPowerSupplyInterface.h
//  Generic Power Supply Interface to be used into CU
//
//  Created by andrea michelotti on 10/24/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#ifndef __ChaosPowerSupplyInterface__
#define __ChaosPowerSupplyInterface__

#include <iostream>
#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>
#include <common/powersupply/core/AbstractPowerSupply.h>
#include <stdint.h>
namespace chaos_driver=::chaos::cu::driver_manager::driver;
namespace chaos {
    namespace driver {
#define MAX_STR_SIZE 256
        namespace powersupply {
            class ChaosPowerSupplyDD;
            typedef enum {
                OP_INIT = chaos_driver::OpcodeType::OP_USER, // init low level driver
                OP_DEINIT, // deinit low level driver
                
                OP_SET_POLARITY, // set polarity
                OP_GET_POLARITY, // get polarity
                OP_SET_SP, // set current set point
                OP_GET_SP, // get current set point
                
                OP_START_RAMP, // start ramp
                OP_GET_VOLTAGE_OUTPUT, // get voltage
                OP_GET_CURRENT_OUTPUT,  // get current
                OP_SET_CURRENT_RAMP_SPEED, //set current ramp speed
                OP_RESET_ALARMS,
                OP_GET_ALARMS,
                OP_SHUTDOWN,
                OP_STANDBY,
                OP_POWERON,
                OP_GET_STATE,
                OP_GET_SWVERSION,
                OP_GET_HWVERSION,
                OP_GET_CURRENT_SENSIBILITY,
                OP_GET_VOLTAGE_SENSIBILITY,
                OP_SET_CURRENT_SENSIBILITY,
                OP_SET_VOLTAGE_SENSIBILITY,
                OP_GET_MAXMIN_CURRENT,
                OP_GET_MAXMIN_VOLTAGE,
                OP_GET_ALARM_DESC,
                OP_FORCE_MAX_CURRENT,
                OP_FORCE_MAX_VOLTAGE,
                OP_GET_FEATURE
            } ChaosPowerSupplyOpcode;
            
            typedef struct {
                float fvalue0;
                float fvalue1;

                int ivalue;
                uint32_t timeout;
                uint64_t alarm_mask;
                
            } powersupply_iparams_t;
            
            typedef struct {
                float fvalue0;
                float fvalue1;
                int ivalue;
                int result;
                uint64_t alarm_mask;
                char str[MAX_STR_SIZE];
            } powersupply_oparams_t;
            
            // wrapper interface
            class ChaosPowerSupplyInterface:public ::common::powersupply::AbstractPowerSupply {
            protected:

                chaos_driver::DrvMsg message;
                
            public:
                ChaosPowerSupplyInterface(chaos_driver::DriverAccessor*_accessor):accessor(_accessor){impl=(ChaosPowerSupplyDD*)_accessor->getImpl();};
		        chaos_driver::DriverAccessor* accessor;
                ChaosPowerSupplyDD*impl;
                /**
                 @brief sets the current polarity
                 @param pol if >0 sets positive current polarity, if <0 sets negative current polarity, =0 opens the circuit, no current
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                
                int setPolarity(int pol,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                /**
                 @brief gets the current polarity
                 @param pol returns the polarity if >0 positive current polarity, if <0 negative current polarity, =0 circuit is open, no current
                 @param polsp returns the polarity SP if >0 positive current polarity, if <0 negative current polarity, =0 circuit is open, no current

                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                
                int getPolarity(int* pol,int*polsp=NULL,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
                /**
                 @brief sets the current set point
                 @param current the current set point to reach expressed in ampere
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                int setCurrentSP(float current,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
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
                int setCurrentRampSpeed(float asup,float asdown,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                /**
                 @brief resets alarms
                 @param alrm a 64 bit field containing the alarms to be reset (-1 all alarms)
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success
                 */
                
                int resetAlarms(uint64_t alrm,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
                
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
                @param statesp returns a bit field of PowerSupplyStates SP

                 @param desc return a string description
                 @param timeo_ms timeout in ms for the completion of the operation (0 wait indefinitively)
                 @return 0 if success or an error code
                 */
                
                int getState(int* state,std::string& desc,int*statesp,uint32_t timeo_ms=POWER_SUPPLY_DEFAULT_TIMEOUT);
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
                 @param max the max current that the power supply can output
                 @return 0 if success or an error code
                 */
                int forceMaxVoltage(float max);
                 /**
                 @brief return a bitfield of capabilities of the powersupply
                 @return the a bit field of capability
                 */
                uint64_t getFeatures() ;


                bool isBypass();
			    void setBypass(bool);

            };
        }
    }
}

namespace chaos_powersupply_dd = chaos::driver::powersupply;
#endif
