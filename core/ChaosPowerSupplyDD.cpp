/*
 *	Power supply base for DD
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
#include "ChaosPowerSupplyDD.h"

#include <string>
#include <boost/regex.hpp>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <driver/powersupply/core/ChaosPowerSupplyDD.h>
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>
using namespace chaos::driver::powersupply;
//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR_WITH_NS(chaos::driver::powersupply, ChaosPowerSupplyDD) {
    power = NULL;
	
}

//default descrutcor
ChaosPowerSupplyDD::~ChaosPowerSupplyDD() {
	
}

void ChaosPowerSupplyDD::driverDeinit() throw(chaos::CException) {
    if(power){
        PSDBG<< "Removing PowerSupply driver "<<std::hex<<power<<std::dec;

        delete power;
    }
    power = NULL;
}


cu_driver::MsgManagmentResultType::MsgManagmentResult ChaosPowerSupplyDD::execOpcode(cu_driver::DrvMsgPtr cmd){
   //     boost::mutex::scoped_lock lock(io_mux);

    cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
    powersupply_iparams_t *in = (powersupply_iparams_t *)cmd->inputData;
    powersupply_oparams_t *out = (powersupply_oparams_t *)cmd->resultData;

    switch(cmd->opcode){
        case OP_INIT:
             PSDBG<< "Initializing";
             out->result = power->initPS();
            break;
            
        case OP_DEINIT:
             PSDBG<< "Deinitializing";
             out->result = power->deinitPS();
            break;
        case OP_SET_POLARITY:
      //      PSDBG<< "Set Polarity to:"<<in->ivalue<<" timeo:"<<in->timeout;
            out->result= power->setPolarity(in->ivalue,in->timeout);
            
            break;
        case OP_GET_POLARITY:
            out->result = power->getPolarity(&out->ivalue,in->timeout);
     //       PSDBG<< "Got Polarity "<<out->ivalue;
            break;
        case OP_SET_SP:
   //         PSDBG<< "Set Current SP to:"<<in->fvalue0<<" timeo:"<<in->timeout;
            out->result = power->setCurrentSP(in->fvalue0,in->timeout);
            break;
            
        case OP_FORCE_MAX_CURRENT:
    //        PSDBG<< "Force max current"<<in->fvalue0;
            out->result = power->forceMaxCurrent(in->fvalue0);
            break;
        case OP_FORCE_MAX_VOLTAGE:
    //        PSDBG<< "Force max voltage"<<in->fvalue0;
            out->result = power->forceMaxVoltage(in->fvalue0);
            break;
       case OP_SET_CURRENT_SENSIBILITY:
     //       PSDBG<< "Set current Sensibility"<<in->fvalue0;
            out->result = power->setCurrentSensibility(in->fvalue0);
            break;
        case OP_SET_VOLTAGE_SENSIBILITY:
     //       PSDBG<< "Set voltage sensibility"<<in->fvalue0;
            out->result = power->setVoltageSensibility(in->fvalue0);
            break;
       
            
        case OP_GET_SP: // get current set point
            out->result = power->getCurrentSP(&out->fvalue0,in->timeout);
     //       PSDBG<< "Got Current SP "<<out->fvalue0;
            break;
        case OP_START_RAMP: // start ramp
     //       PSDBG<< "Start Ramp timeo:"<<in->timeout;
            out->result = power->startCurrentRamp(in->timeout);
            break;
        case OP_GET_VOLTAGE_OUTPUT:
            out->result = power->getVoltageOutput(&out->fvalue0,in->timeout);
    //        PSDBG<< "Got Voltage "<<out->fvalue0;
            break;
        case OP_GET_CURRENT_OUTPUT:
            out->result = power->getCurrentOutput(&out->fvalue0,in->timeout);
    ///        PSDBG<< "Got Current "<<out->fvalue0;
            break;
        case OP_SET_CURRENT_RAMP_SPEED:
            PSDBG<<"Setting current ramp speed min:"<<in->fvalue0<<" max:"<<in->fvalue1<<" timeout:"<<in->timeout;
            out->result = power->setCurrentRampSpeed(in->fvalue0,in->fvalue1,in->timeout);
            break;
        case OP_RESET_ALARMS:
            PSDBG<<"Reset alarms to:"<<in->alarm_mask<<" timeout:"<<in->timeout;
            out->result = power->resetAlarms(in->alarm_mask,in->timeout);
            break;
        case OP_GET_ALARMS:
            out->result = power->getAlarms(&out->alarm_mask,in->timeout);
   //         PSDBG<<"Got alarms to:"<<out->alarm_mask<<" timeout:"<<in->timeout;
            break;
            
        case OP_GET_FEATURE:{
            uint64_t feat=power->getFeatures();
            out->alarm_mask=feat;
    //        PSDBG<<"Got Features:"<<feat;
        }
            break;
        case OP_SHUTDOWN:
            PSDBG<<"Shutting down"<<" timeout:"<<in->timeout;
            out->result = power->shutdown(in->timeout);
            break;
        case OP_STANDBY:
            PSDBG<<"Standby "<<" timeout:"<<in->timeout;
            out->result = power->standby(in->timeout);
            break;
        case OP_POWERON:
            PSDBG<<"Poweron "<<" timeout:"<<in->timeout;
            out->result = power->poweron(in->timeout);
            break;
        case OP_GET_STATE:{
            std::string desc;
            out->result = power->getState(&out->ivalue,desc,in->timeout);
            strncpy(out->str,desc.c_str(),MAX_STR_SIZE);
      //      PSDBG<<"Got State: "<<out->ivalue<<" \""<<desc<<"\" timeout:"<<in->timeout;
            break;
        }
        case OP_GET_SWVERSION:{
            std::string ver;
            out->result = power->getSWVersion(ver,in->timeout);
            PSDBG<<"Got HW Version:\""<<ver<<"\" timeout:"<<in->timeout;
            strncpy(out->str,ver.c_str(),MAX_STR_SIZE);;

            break;
        }
        case OP_GET_HWVERSION:{
            std::string ver;
            out->result = power->getHWVersion(ver,in->timeout);
            PSDBG<<"Got SW Version:\""<<ver<<"\" timeout:"<<in->timeout;
            strncpy(out->str,ver.c_str(),MAX_STR_SIZE);;

            break;
        }
        case OP_GET_CURRENT_SENSIBILITY:
            out->result = power->getCurrentSensibility(&out->fvalue0);
     //       PSDBG<<"Got Current sensibility: \""<<out->fvalue0<<"\"";
            break;
        case OP_GET_VOLTAGE_SENSIBILITY:
            out->result = power->getVoltageSensibility(&out->fvalue0);
      //      PSDBG<<"Got Voltage sensibility: \""<<out->fvalue0<<"\"";
            break;
        
        case OP_GET_MAXMIN_CURRENT:
            out->result = power->getMaxMinCurrent(&out->fvalue0,&out->fvalue1);
       //     PSDBG<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" current";

            break;
        case OP_GET_MAXMIN_VOLTAGE:
            out->result = power->getMaxMinVoltage(&out->fvalue0,&out->fvalue1);
     //       PSDBG<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" voltage";
            break;
        case OP_GET_ALARM_DESC:
            out->result = power->getAlarmDesc(&out->alarm_mask);
     //       PSDBG<<"Got Alarm maxk "<<out->alarm_mask;

            break;
        default:
            PSERR<<"Opcode not supported:"<<cmd->opcode;
    }
    return result;
}


int ChaosPowerSupplyDD::setPolarity(int pol,uint32_t timeo_ms){
    return power->setPolarity(pol,timeo_ms);
   // WRITE_OP_INT_TIM(OP_SET_POLARITY, pol, timeo_ms);
}


int ChaosPowerSupplyDD::getPolarity(int* pol,uint32_t timeo_ms){
   // READ_OP_INT_TIM(OP_GET_POLARITY, pol, timeo_ms);
       return power->getPolarity(pol,timeo_ms);

}

int ChaosPowerSupplyDD::setCurrentSP(float current,uint32_t timeo_ms){
   // WRITE_OP_FLOAT_TIM(OP_SET_SP, current, timeo_ms);
   return power->setCurrentSP(current);
}

int ChaosPowerSupplyDD::forceMaxCurrent(float max){
    //WRITE_OP_FLOAT_TIM(OP_FORCE_MAX_CURRENT, max,chaos::common::constants::CUTimersTimeoutinMSec);
    return 0;
}


int ChaosPowerSupplyDD::forceMaxVoltage(float max){
  //  WRITE_OP_FLOAT_TIM(OP_FORCE_MAX_VOLTAGE, max,chaos::common::constants::CUTimersTimeoutinMSec);
  return 0;

}

int ChaosPowerSupplyDD::setCurrentSensibility(float max){
  //  WRITE_OP_FLOAT_TIM(OP_SET_CURRENT_SENSIBILITY, max,chaos::common::constants::CUTimersTimeoutinMSec);
  return 0;

}
int ChaosPowerSupplyDD::setVoltageSensibility(float max){
    //WRITE_OP_FLOAT_TIM(OP_SET_VOLTAGE_SENSIBILITY, max,chaos::common::constants::CUTimersTimeoutinMSec);
    return 0;

}


int ChaosPowerSupplyDD::getCurrentSP(float* current,uint32_t timeo_ms){
    //READ_OP_FLOAT_TIM(OP_GET_SP, current, timeo_ms);
    return power->getCurrentSP(current,timeo_ms);

}

int ChaosPowerSupplyDD::startCurrentRamp(uint32_t timeo_ms){
    // WRITE_OP_TIM(OP_START_RAMP,timeo_ms);
    return power->startCurrentRamp(timeo_ms);
}


int ChaosPowerSupplyDD::getVoltageOutput(float* volt,uint32_t timeo_ms){
   // READ_OP_FLOAT_TIM(OP_GET_VOLTAGE_OUTPUT, volt, timeo_ms);
   return power->getVoltageOutput(volt,timeo_ms);
}

int ChaosPowerSupplyDD::getCurrentOutput(float* current,uint32_t timeo_ms){
  //  READ_OP_FLOAT_TIM(OP_GET_CURRENT_OUTPUT, current, timeo_ms);
    return power->getCurrentOutput(current,timeo_ms);
}
int ChaosPowerSupplyDD::setCurrentRampSpeed(float asup,float asdown,uint32_t timeo_ms){
  //  WRITE_OP_2FLOAT_TIM(OP_SET_CURRENT_RAMP_SPEED, asup,asdown, timeo_ms);
  return power->setCurrentRampSpeed(asup,asdown,timeo_ms);
}

int ChaosPowerSupplyDD::resetAlarms(uint64_t alrm,uint32_t timeo_ms){
    //WRITE_OP_64INT_TIM(OP_RESET_ALARMS,alrm,timeo_ms);
    return power->resetAlarms(alrm,timeo_ms);
}

int ChaosPowerSupplyDD::getAlarms(uint64_t*alrm,uint32_t timeo_ms){
    //READ_OP_64INT_TIM(OP_GET_ALARMS,alrm,timeo_ms);
    return power->getAlarms(alrm,timeo_ms);
}

int ChaosPowerSupplyDD::shutdown(uint32_t timeo_ms){
   // WRITE_OP_TIM(OP_SHUTDOWN,timeo_ms);
   return power->shutdown(timeo_ms);
}

int ChaosPowerSupplyDD::standby(uint32_t timeo_ms){
   // WRITE_OP_TIM(OP_STANDBY,timeo_ms);
   return power->standby(timeo_ms);

}

int ChaosPowerSupplyDD::poweron(uint32_t timeo_ms){
    //WRITE_OP_TIM(OP_POWERON,timeo_ms);
    return power->poweron(timeo_ms);
}

int ChaosPowerSupplyDD::getState(int* state,std::string& desc,uint32_t timeo_ms){
   // READ_OP_INT_STRING_TIM(OP_GET_STATE, state, desc,timeo_ms);
   return power->getState(state,desc,timeo_ms);

}

int ChaosPowerSupplyDD::initPS(){
   // WRITE_OP_TIM(OP_INIT,chaos::common::constants::CUTimersTimeoutinMSec);
   return power->initPS();
}

int ChaosPowerSupplyDD::deinitPS(){
    //WRITE_OP_TIM(OP_DEINIT,chaos::common::constants::CUTimersTimeoutinMSec);
    return power->deinitPS();

}
int ChaosPowerSupplyDD::getSWVersion(std::string& ver,uint32_t timeo_ms){
    int state;
    //READ_OP_INT_STRING_TIM(OP_GET_SWVERSION, &state, ver,timeo_ms);
    return power->getSWVersion(ver,timeo_ms);
}

int ChaosPowerSupplyDD::getHWVersion(std::string&ver,uint32_t timeo_ms){
    int state;
 //   READ_OP_INT_STRING_TIM(OP_GET_HWVERSION, &state, ver,timeo_ms);
    return power->getHWVersion(ver,timeo_ms);
}

int ChaosPowerSupplyDD::getCurrentSensibility(float*sens){
  //  READ_OP_FLOAT_TIM(OP_GET_CURRENT_SENSIBILITY, sens,chaos::common::constants::CUTimersTimeoutinMSec);
    return 0;
}

int ChaosPowerSupplyDD::getVoltageSensibility(float*sens){
   // READ_OP_FLOAT_TIM(OP_GET_VOLTAGE_SENSIBILITY, sens,chaos::common::constants::CUTimersTimeoutinMSec);
   return 0;
}

int ChaosPowerSupplyDD::getMaxMinCurrent(float*max,float*min){
    //READ_OP_2FLOAT_TIM(OP_GET_MAXMIN_CURRENT,max,min,chaos::common::constants::CUTimersTimeoutinMSec);
    return power->getMaxMinCurrent(max,min);
}
int ChaosPowerSupplyDD::getMaxMinVoltage(float*max,float*min){
    //READ_OP_2FLOAT_TIM(OP_GET_MAXMIN_VOLTAGE,max,min,chaos::common::constants::CUTimersTimeoutinMSec);
    return power->getMaxMinVoltage(max,min);

}
int ChaosPowerSupplyDD::getAlarmDesc(uint64_t *desc){
   // READ_OP_64INT_TIM(OP_GET_ALARM_DESC,desc,chaos::common::constants::CUTimersTimeoutinMSec);
    return power->getAlarmDesc(desc);
}
uint64_t ChaosPowerSupplyDD::getFeatures() {
    uint64_t feats=0;
    //READ_OP_64INT_TIM_NORET(OP_GET_FEATURE,&feats,chaos::common::constants::CUTimersTimeoutinMSec);
    //return feats;
    return power->getFeatures();
}





