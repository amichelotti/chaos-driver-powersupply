//
//  ChaosPowerSupplyInterface.cpp
//  OcemE642X
//
//  Created by andrea michelotti on 10/24/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "ChaosPowerSupplyInterface.h"
using namespace chaos::driver::powersupply;

#define RETURN \
    {int tmp=ret->result;free(message.inputData);free(message.resultData);return tmp;}

#define RETURN_VOID \
    {free(message.inputData);free(message.resultData);}

#define SEND_AND_RETURN \
 accessor->send(&message);			\
RETURN

#define SEND_AND_RETURN_TIM(t) \
 accessor->send(&message,t);			\
RETURN

#define PREPARE_OP_RET_INT_TIMEOUT(op,tim) \
powersupply_oparams_t* ret=(powersupply_oparams_t*)calloc(1,sizeof(powersupply_oparams_t));\
powersupply_iparams_t* idata=(powersupply_iparams_t*)calloc(1,sizeof(powersupply_iparams_t));\
ret->result=DRV_BYPASS_DEFAULT_CODE;\
message.opcode = op; \
message.inputData=(void*)idata;\
idata->timeout=tim;\
message.inputDataLength=sizeof(powersupply_iparams_t);\
message.resultDataLength=sizeof(powersupply_oparams_t);\
message.resultData = (void*)ret;

#define WRITE_OP_INT_TIM(op,ival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata->ivalue=ival;\
SEND_AND_RETURN_TIM(timeout)

#define WRITE_OP_64INT_TIM(op,ival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata->alarm_mask=ival;\
SEND_AND_RETURN_TIM(timeout)

#define WRITE_OP_FLOAT_TIM(op,fval,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata->fvalue0=fval;\
SEND_AND_RETURN_TIM(timeout)

#define WRITE_OP_2FLOAT_TIM(op,fval0,fval1,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata->fvalue0=fval0;\
idata->fvalue1=fval1;\
SEND_AND_RETURN_TIM(timeout)

#define READ_OP_FLOAT_TIM(op,pfval,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout);\
ret->fvalue0=*pfval;\
ret->fvalue0=*pfval;\
accessor->send(&message,timeout);\
*pfval = ret->fvalue0;\
SEND_AND_RETURN

#define READ_OP_INT_TIM(op,pival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
ret->ivalue=*pival;\
accessor->send(&message,timeout);\
*pival = ret->ivalue;\
SEND_AND_RETURN

#define READ_OP_INT_STRING_TIM(op,pival,pstring,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
*ret->str=0;\
ret->ivalue=*pival;\
accessor->send(&message,timeout);\
*pival = ret->ivalue;\
pstring = ret->str;\
SEND_AND_RETURN

#define READ_OP_INT_TIM(op,pival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
ret->ivalue=*pival;\
accessor->send(&message,timeout);\
*pival = ret->ivalue;\
SEND_AND_RETURN

#define READ_OP_64INT_TIM(op,pival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
ret->alarm_mask= *pival;\
accessor->send(&message,timeout);\
*pival = ret->alarm_mask;\
SEND_AND_RETURN

#define READ_OP_64INT_TIM_NORET(op,pival,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
ret->alarm_mask= *pival;\
accessor->send(&message,timeout);\
*pival = ret->alarm_mask;\
RETURN_VOID

#define READ_OP_2FLOAT_TIM(op,pfval0,pfval1,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
ret->fvalue0=*pfval0 ;\
ret->fvalue1=*pfval1 ;\
accessor->send(&message,timeout);\
*pfval0 = ret->fvalue0;\
*pfval1 = ret->fvalue1;\
RETURN

#define WRITE_OP_TIM(op,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
SEND_AND_RETURN_TIM(timeout)

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
    WRITE_OP_FLOAT_TIM(OP_FORCE_MAX_CURRENT, max,chaos::common::constants::CUTimersTimeoutinMSec);
}


int ChaosPowerSupplyInterface::forceMaxVoltage(float max){
    WRITE_OP_FLOAT_TIM(OP_FORCE_MAX_VOLTAGE, max,chaos::common::constants::CUTimersTimeoutinMSec);

}

int ChaosPowerSupplyInterface::setCurrentSensibility(float max){
    WRITE_OP_FLOAT_TIM(OP_SET_CURRENT_SENSIBILITY, max,chaos::common::constants::CUTimersTimeoutinMSec);

}
int ChaosPowerSupplyInterface::setVoltageSensibility(float max){
    WRITE_OP_FLOAT_TIM(OP_SET_VOLTAGE_SENSIBILITY, max,chaos::common::constants::CUTimersTimeoutinMSec);

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
    WRITE_OP_TIM(OP_INIT,chaos::common::constants::CUTimersTimeoutinMSec);
}

int ChaosPowerSupplyInterface::deinit(){
    WRITE_OP_TIM(OP_DEINIT,chaos::common::constants::CUTimersTimeoutinMSec);

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
    READ_OP_FLOAT_TIM(OP_GET_CURRENT_SENSIBILITY, sens,chaos::common::constants::CUTimersTimeoutinMSec);
}

int ChaosPowerSupplyInterface::getVoltageSensibility(float*sens){
    READ_OP_FLOAT_TIM(OP_GET_VOLTAGE_SENSIBILITY, sens,chaos::common::constants::CUTimersTimeoutinMSec);
}

int ChaosPowerSupplyInterface::getMaxMinCurrent(float*max,float*min){
    READ_OP_2FLOAT_TIM(OP_GET_MAXMIN_CURRENT,max,min,chaos::common::constants::CUTimersTimeoutinMSec);
}
int ChaosPowerSupplyInterface::getMaxMinVoltage(float*max,float*min){
    READ_OP_2FLOAT_TIM(OP_GET_MAXMIN_VOLTAGE,max,min,chaos::common::constants::CUTimersTimeoutinMSec);

}
int ChaosPowerSupplyInterface::getAlarmDesc(uint64_t *desc){
    READ_OP_64INT_TIM(OP_GET_ALARM_DESC,desc,chaos::common::constants::CUTimersTimeoutinMSec);

}
uint64_t ChaosPowerSupplyInterface::getFeatures() {
    uint64_t feats=0;
    READ_OP_64INT_TIM_NORET(OP_GET_FEATURE,&feats,chaos::common::constants::CUTimersTimeoutinMSec);
    return feats;
}

