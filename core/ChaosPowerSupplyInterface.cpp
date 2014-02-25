//
//  ChaosPowerSupplyInterface.cpp
//  OcemE642X
//
//  Created by andrea michelotti on 10/24/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "ChaosPowerSupplyInterface.h"
using namespace chaos::driver::powersupply;

#define PREPARE_OP_RET_INT_TIMEOUT(op,tim) \
powersupply_oparams_t ret;\
powersupply_iparams_t idata;\
message.opcode = op; \
message.inputData=(void*)&idata;\
idata.timeout=tim;\
message.inputDataLength=sizeof(powersupply_iparams_t);\
message.resultDataLength=sizeof(powersupply_oparams_t);\
message.resultData = (void*)&ret;\

#define WRITE_OP_INT_TIM(op,ival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.ivalue=ival;\
 accessor->send(&message,100);			\
return ret.result;

#define WRITE_OP_64INT_TIM(op,ival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.alarm_mask=ival;\
accessor->send(&message);\
return ret.result;

#define WRITE_OP_FLOAT_TIM(op,fval,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.fvalue0=fval;\
 accessor->send(&message,100);			\
return ret.result;

#define WRITE_OP_2FLOAT_TIM(op,fval0,fval1,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.fvalue0=fval0;\
idata.fvalue1=fval1;\
accessor->send(&message);\
return ret.result;

#define READ_OP_FLOAT_TIM(op,pfval,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
*pfval = ret.fvalue0;\
return ret.result;

#define READ_OP_INT_TIM(op,pival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
*pival = ret.ivalue;\
return ret.result;

#define READ_OP_INT_STRING_TIM(op,pival,pstring,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
*pival = ret.ivalue;\
pstring = ret.str;\
return ret.result;

#define READ_OP_INT_TIM(op,pival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
*pival = ret.ivalue;\
return ret.result;

#define READ_OP_64INT_TIM(op,pival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
*pival = ret.alarm_mask;\
return ret.result;

#define READ_OP_2FLOAT_TIM(op,pfval0,pfval1,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
*pfval0 = ret.fvalue0;\
*pfval1 = ret.fvalue1;\
return ret.result;

#define WRITE_OP_TIM(op,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
return ret.result;


int ChaosPowerSupplyInterface::setPolarity(int pol,uint32_t timeo_ms){
    WRITE_OP_INT_TIM(OP_SET_POLARITY, pol, timeo_ms);
}


int ChaosPowerSupplyInterface::getPolarity(int* pol,uint32_t timeo_ms){
    READ_OP_INT_TIM(OP_GET_POLARITY, pol, timeo_ms);
}

int ChaosPowerSupplyInterface::setCurrentSP(float current,uint32_t timeo_ms){
    WRITE_OP_FLOAT_TIM(OP_SET_SP, current, timeo_ms);
}

int ChaosPowerSupplyInterface::forceMaxCurrent(float max){
    WRITE_OP_FLOAT_TIM(OP_FORCE_MAX_CURRENT, max, timeo_ms);
}


int ChaosPowerSupplyInterface::forceMaxVoltage(float max){
    WRITE_OP_FLOAT_TIM(OP_FORCE_MAX_VOLTAGE, max, timeo_ms);

}

int ChaosPowerSupplyInterface::getCurrentSP(float* current,uint32_t timeo_ms){
    READ_OP_FLOAT_TIM(OP_GET_SP, current, timeo_ms);
}

int ChaosPowerSupplyInterface::startCurrentRamp(uint32_t timeo_ms){
    WRITE_OP_TIM(OP_START_RAMP,timeo_ms);
}


int ChaosPowerSupplyInterface::getVoltageOutput(float* volt,uint32_t timeo_ms){
    READ_OP_FLOAT_TIM(OP_GET_VOLTAGE_OUTPUT, volt, timeo_ms);
}

int ChaosPowerSupplyInterface::getCurrentOutput(float* current,uint32_t timeo_ms){
    READ_OP_FLOAT_TIM(OP_GET_CURRENT_OUTPUT, current, timeo_ms);

}
int ChaosPowerSupplyInterface::setCurrentRampSpeed(float asup,float asdown,uint32_t timeo_ms){
    WRITE_OP_2FLOAT_TIM(OP_SET_CURRENT_RAMP_SPEED, asup,asdown, timeo_ms);
}

int ChaosPowerSupplyInterface::resetAlarms(uint64_t alrm,uint32_t timeo_ms){
    WRITE_OP_64INT_TIM(OP_RESET_ALARMS,alrm,timeo_ms);
}

int ChaosPowerSupplyInterface::getAlarms(uint64_t*alrm,uint32_t timeo_ms){
    READ_OP_64INT_TIM(OP_GET_ALARMS,alrm,timeo_ms);
}

int ChaosPowerSupplyInterface::shutdown(uint32_t timeo_ms){
    WRITE_OP_TIM(OP_SHUTDOWN,timeo_ms);
}

int ChaosPowerSupplyInterface::standby(uint32_t timeo_ms){
    WRITE_OP_TIM(OP_STANDBY,timeo_ms);

}

int ChaosPowerSupplyInterface::poweron(uint32_t timeo_ms){
    WRITE_OP_TIM(OP_POWERON,timeo_ms);

}

int ChaosPowerSupplyInterface::getState(int* state,std::string& desc,uint32_t timeo_ms){
    READ_OP_INT_STRING_TIM(OP_GET_STATE, state, desc,timeo_ms);

}

int ChaosPowerSupplyInterface::init(){
    WRITE_OP_TIM(OP_INIT,0);
}

int ChaosPowerSupplyInterface::deinit(){
    WRITE_OP_TIM(OP_DEINIT,0);

}
int ChaosPowerSupplyInterface::getSWVersion(std::string& ver,uint32_t timeo_ms){
    int state;
    READ_OP_INT_STRING_TIM(OP_GET_SWVERSION, &state, ver,timeo_ms);
}

int ChaosPowerSupplyInterface::getHWVersion(std::string&ver,uint32_t timeo_ms){
    int state;
    READ_OP_INT_STRING_TIM(OP_GET_HWVERSION, &state, ver,timeo_ms);

}

int ChaosPowerSupplyInterface::getCurrentSensibility(float*sens){
    READ_OP_FLOAT_TIM(OP_GET_CURRENT_SENSIBILITY, sens, 0);
}

int ChaosPowerSupplyInterface::getVoltageSensibility(float*sens){
    READ_OP_FLOAT_TIM(OP_GET_VOLTAGE_SENSIBILITY, sens, 0);
}

int ChaosPowerSupplyInterface::getMaxMinCurrent(float*max,float*min){
    READ_OP_2FLOAT_TIM(OP_GET_MAXMIN_CURRENT,max,min,0);
}
int ChaosPowerSupplyInterface::getMaxMinVoltage(float*max,float*min){
    READ_OP_2FLOAT_TIM(OP_GET_MAXMIN_VOLTAGE,max,min,0);

}
int ChaosPowerSupplyInterface::getAlarmDesc(uint64_t *desc){
    READ_OP_64INT_TIM(OP_GET_ALARM_DESC,desc,0);

}
