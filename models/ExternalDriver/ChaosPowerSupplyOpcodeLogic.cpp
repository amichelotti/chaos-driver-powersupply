/*
 *	ChaosPowerSupplyOpcodeLogic.cpp
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

#include "ChaosPowerSupplyOpcodeLogic.h"
#include "../../core/ChaosPowerSupplyInterface.h"

#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/bson/util/base64.h>
#define INFO INFO_LOG(ChaosPowerSupplyOpcodeLogic)
#define ERR ERR_LOG(ChaosPowerSupplyOpcodeLogic)
#define DBG DBG_LOG(ChaosPowerSupplyOpcodeLogic)

using namespace chaos::driver::powersupply;
using namespace chaos::common::data;
using namespace chaos::cu::driver_manager::driver;
ChaosPowerSupplyOpcodeLogic::ChaosPowerSupplyOpcodeLogic(chaos::cu::driver_manager::driver::RemoteIODriverProtocol *_remote_driver):
OpcodeExternalCommandMapper(_remote_driver){}

ChaosPowerSupplyOpcodeLogic::~ChaosPowerSupplyOpcodeLogic() {}

void ChaosPowerSupplyOpcodeLogic::driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
    INFO << init_parameter.getJSONString();
    if(init_parameter.hasKey("powersupply_init_param") == false) {
        throw CException(-1, "No init parameter", __PRETTY_FUNCTION__);
    }
    if(init_parameter.isCDataWrapperValue("powersupply_init_param") == false) {
        throw CException(-1, "Init parameter need to be a json object", __PRETTY_FUNCTION__);
    }
    powersupply_init_pack.reset(init_parameter.getCSDataValue("powersupply_init_param"));
}

void ChaosPowerSupplyOpcodeLogic::driverDeinit() throw(chaos::CException) {
    
}

int ChaosPowerSupplyOpcodeLogic::asyncMessageReceived(CDWUniquePtr message) {
    INFO << message->getJSONString();
    return 0;
}

#define WRITE_ERR_ON_CMD(r, c, m, d)\
cmd->ret = c;\
snprintf(cmd->err_msg, 255, "%s", m);\
snprintf(cmd->err_dom, 255, "%s", d);

#define RETURN_ERROR(r, c, m, d)\
WRITE_ERR_ON_CMD(r, c, m, d)\
return cmd->ret;

#define SEND_REQUEST(c, r,a)\
if(sendRawRequest(ChaosMoveOperator(r), a)) {\
WRITE_ERR_ON_CMD(c, -1, "Timeout waiting answer from remote driver", __PRETTY_FUNCTION__);\
} else {\
if(response->hasKey("err")) {\
if(response->isInt32Value("err") == false)  {\
WRITE_ERR_ON_CMD(c, -3, "'err' key need to be an int32 value", __PRETTY_FUNCTION__);\
} else {\
c->ret = response->getInt32Value("err");\
}\
} else {\
WRITE_ERR_ON_CMD(c, -2, "'err' key not found on external driver return package", __PRETTY_FUNCTION__);\
}\
}
#define CHECK_KEY_IN_RESPONSE(r, k, t, e1, e2)\
if(!r->hasKey(k)) {\
    std::string es1 = CHAOS_FORMAT("'%1%' key is mandatory in remote driver response",%k);\
    RETURN_ERROR(cmd, e1, es1.c_str(), __PRETTY_FUNCTION__);\
} else if(!r->t(k) == 0) {\
    std::string es2 = CHAOS_FORMAT("'%1%' key in remote driver response need to be int32 value", %k);\
    RETURN_ERROR(cmd, e2, es2.c_str(), __PRETTY_FUNCTION__);\
}

int ChaosPowerSupplyOpcodeLogic::sendInit(DrvMsgPtr cmd) {
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "init");
    init_pack->addCSDataValue("par", *powersupply_init_pack);
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    return  cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::sendDeinit(DrvMsgPtr cmd) {
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "deinit");
    init_pack->addCSDataValue("par", *powersupply_init_pack);
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::setPolarity(DrvMsgPtr cmd, int pol,uint32_t timeo_ms) {
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getPolarity(DrvMsgPtr cmd, int* pol,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentSP(DrvMsgPtr cmd, float current,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentSP(DrvMsgPtr cmd, float* current,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::startCurrentRamp(DrvMsgPtr cmd, uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getVoltageOutput(DrvMsgPtr cmd, float* volt,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentOutput(DrvMsgPtr cmd, float* current,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentRampSpeed(DrvMsgPtr cmd, float asup,float asdown,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::resetAlarms(DrvMsgPtr cmd, uint64_t alrm,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getAlarms(DrvMsgPtr cmd, uint64_t*alrm,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::shutdown(DrvMsgPtr cmd, uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::standby(DrvMsgPtr cmd, uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::poweron(DrvMsgPtr cmd, uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getState(DrvMsgPtr cmd, int* state,std::string& desc,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "get_state");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", isInt32Value, -1, -2);
    CHECK_KEY_IN_RESPONSE(response, "description", isStringValue, -3, -4);
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getSWVersion(DrvMsgPtr cmd, std::string& version,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getHWVersion(DrvMsgPtr cmd, std::string& version,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentSensibility(DrvMsgPtr cmd, float *sens){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getVoltageSensibility(DrvMsgPtr cmd, float *sens){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentSensibility(DrvMsgPtr cmd, float sens){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::setVoltageSensibility(DrvMsgPtr cmd, float sens){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getMaxMinCurrent(DrvMsgPtr cmd, float*max,float*min){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getMaxMinVoltage(DrvMsgPtr cmd, float*max,float*min){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getAlarmDesc(DrvMsgPtr cmd, uint64_t* alarm){
   return 0;
}

int ChaosPowerSupplyOpcodeLogic::forceMaxCurrent(DrvMsgPtr cmd, float max){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::forceMaxVoltage(DrvMsgPtr cmd, float max){
    return 0;
}

uint64_t ChaosPowerSupplyOpcodeLogic::getFeatures(DrvMsgPtr cmd) {
    return 0;
}

//! Execute a command
MsgManagmentResultType::MsgManagmentResult ChaosPowerSupplyOpcodeLogic::execOpcode(DrvMsgPtr cmd) {
    MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
    powersupply_iparams_t *in = (powersupply_iparams_t *)cmd->inputData;
    powersupply_oparams_t *out = (powersupply_oparams_t *)cmd->resultData;
    cmd->ret = 0;
    memset(cmd->err_msg, 0, 255);
    memset(cmd->err_dom, 0, 255);
    switch(cmd->opcode) {
        case OP_INIT:{
            out->result = sendInit(cmd);
            break;
        }
        case OP_DEINIT:{
            out->result = sendDeinit(cmd);
            break;
        }
        case OP_SET_POLARITY:{
            break;
        }
        case OP_GET_POLARITY:{
            break;
        }
        case OP_SET_SP:{//set current set point
            break;
        }
        case OP_GET_SP:{
            break;
        }
        case OP_START_RAMP:{
            break;
        }
        case OP_GET_VOLTAGE_OUTPUT:{
            break;
        }
        case OP_GET_CURRENT_OUTPUT:{
            break;
        }
        case OP_SET_CURRENT_RAMP_SPEED:{
            break;
        }
        case OP_RESET_ALARMS:{
            break;
        }
        case OP_GET_ALARMS:{
            break;
        }
        case OP_SHUTDOWN:{
            break;
        }
        case OP_STANDBY:{
            break;
        }
        case OP_POWERON:{
            break;
        }
        case OP_GET_STATE:{
            std::string desc;
            out->result = getState(cmd, &out->ivalue,desc,in->timeout);
            strncpy(out->str,desc.c_str(),MAX_STR_SIZE);
            break;
        }
        case OP_GET_SWVERSION:{
            break;
        }
        case OP_GET_HWVERSION:{
            break;
        }
        case OP_GET_CURRENT_SENSIBILITY:{
            break;
        }
        case OP_SET_VOLTAGE_SENSIBILITY:{
            break;
        }
        case OP_GET_MAXMIN_CURRENT:{
            break;
        }
        case OP_GET_MAXMIN_VOLTAGE:{
            break;
        }
        case OP_GET_ALARM_DESC:{
            break;
        }
        case OP_FORCE_MAX_CURRENT:{
            break;
        }
        case OP_FORCE_MAX_VOLTAGE:{
            break;
        }
        case OP_GET_FEATURE:{
            break;
        }
    }
    return result;
}
