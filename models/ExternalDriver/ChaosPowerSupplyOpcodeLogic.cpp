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

int ChaosPowerSupplyOpcodeLogic::setPolarity(DrvMsgPtr cmd, int pol, uint32_t timeo_ms) {
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    CDWUniquePtr set_pola_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "set_pola");
    set_pola_pack->addInt32Value("value", pol);
    init_pack->addCSDataValue("par", *set_pola_pack);
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getPolarity(DrvMsgPtr cmd, int* pol,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "get_pola");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", isInt32Value, -1, -2);
    *pol = response->getInt32Value("value");
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentSP(DrvMsgPtr cmd, float current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    CDWUniquePtr set_cur_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "set_cur");
    set_cur_pack->addDoubleValue("value", current);
    init_pack->addCSDataValue("par", *set_cur_pack);
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentSP(DrvMsgPtr cmd, float* current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "get_cur");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", isDoubleValue, -1, -2);
    *current = response->getDoubleValue("value");
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::startCurrentRamp(DrvMsgPtr cmd, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "start_ramp");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getVoltageOutput(DrvMsgPtr cmd, float* volt,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "get_vol");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", isDoubleValue, -1, -2);
    *volt = response->getDoubleValue("value");
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentOutput(DrvMsgPtr cmd, float* current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "get_cur");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", isDoubleValue, -1, -2);
    *current = response->getDoubleValue("value");
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentRampSpeed(DrvMsgPtr cmd, float asup, float asdown, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    CDWUniquePtr set_slope_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "set_slope");
    set_slope_pack->addDoubleValue("up", asup);
    set_slope_pack->addDoubleValue("uptown", asdown);
    init_pack->addCSDataValue("par", *set_slope_pack);
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::resetAlarms(DrvMsgPtr cmd, uint64_t alrm,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    CDWUniquePtr set_slope_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "rst_alarm");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getAlarms(DrvMsgPtr cmd, uint64_t*alrm,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "get_alarm");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", isDoubleValue, -1, -2);
    *alrm = response->getUInt64Value("value");
    return cmd->ret;
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
    *state = response->getInt32Value("value");
    desc = response->getStringValue("description");
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getSWVersion(DrvMsgPtr cmd, std::string& version,uint32_t timeo_ms){
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getHWVersion(DrvMsgPtr cmd, std::string& version, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addStringValue("opc", "get_hw_ver");
    SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){INFO << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", isStringValue, -1, -2);
    version = response->getStringValue("value");
    return cmd->ret;
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
        case OP_SET_POLARITY:
            DBG<< "Set Polarity to:"<<in->ivalue<<" timeo:"<<in->timeout;
            out->result= setPolarity(cmd, in->ivalue,in->timeout);
            
            break;
        case OP_GET_POLARITY:
            out->result = getPolarity(cmd, &out->ivalue,in->timeout);
            DBG<< "Got Polarity "<<out->ivalue;
            break;
        case OP_SET_SP:
            DBG<< "Set Current SP to:"<<in->fvalue0<<" timeo:"<<in->timeout;
            out->result = setCurrentSP(cmd, in->fvalue0,in->timeout);
            break;
            
        case OP_FORCE_MAX_CURRENT:
            DBG<< "Force max current"<<in->fvalue0;
            out->result = forceMaxCurrent(cmd, in->fvalue0);
            break;
        case OP_FORCE_MAX_VOLTAGE:
            DBG<< "Force max voltage"<<in->fvalue0;
            out->result = forceMaxVoltage(cmd, in->fvalue0);
            break;
        case OP_SET_CURRENT_SENSIBILITY:
            DBG<< "Set current Sensibility"<<in->fvalue0;
            out->result = setCurrentSensibility(cmd, in->fvalue0);
            break;
        case OP_SET_VOLTAGE_SENSIBILITY:
            DBG<< "Set voltage sensibility"<<in->fvalue0;
            out->result = setVoltageSensibility(cmd, in->fvalue0);
            break;
            
            
        case OP_GET_SP: // get current set point
            out->result = getCurrentSP(cmd, &out->fvalue0,in->timeout);
            DBG<< "Got Current SP "<<out->fvalue0;
            break;
        case OP_START_RAMP: // start ramp
            DBG<< "Start Ramp timeo:"<<in->timeout;
            out->result = startCurrentRamp(cmd, in->timeout);
            break;
        case OP_GET_VOLTAGE_OUTPUT:
            out->result = getVoltageOutput(cmd, &out->fvalue0,in->timeout);
            DBG<< "Got Voltage "<<out->fvalue0;
            break;
        case OP_GET_CURRENT_OUTPUT:
            out->result = getCurrentOutput(cmd, &out->fvalue0,in->timeout);
            DBG<< "Got Current "<<out->fvalue0;
            break;
        case OP_SET_CURRENT_RAMP_SPEED:
            DBG<<"Setting current ramp speed min:"<<in->fvalue0<<" max:"<<in->fvalue1<<" timeout:"<<in->timeout;
            out->result = setCurrentRampSpeed(cmd, in->fvalue0,in->fvalue1,in->timeout);
            break;
        case OP_RESET_ALARMS:
            DBG<<"Reset alarms to:"<<in->alarm_mask<<" timeout:"<<in->timeout;
            out->result = resetAlarms(cmd, in->alarm_mask,in->timeout);
            break;
        case OP_GET_ALARMS:
            out->result = getAlarms(cmd, &out->alarm_mask,in->timeout);
            DBG<<"Got alarms to:"<<out->alarm_mask<<" timeout:"<<in->timeout;
            break;
            
        case OP_GET_FEATURE:{
            uint64_t feat=getFeatures(cmd);
            out->alarm_mask=feat;
            DBG<<"Got Features:"<<feat;
        }
            break;
        case OP_SHUTDOWN:
            DBG<<"Shutting down"<<" timeout:"<<in->timeout;
            out->result = shutdown(cmd, in->timeout);
            break;
        case OP_STANDBY:
            DBG<<"Standby "<<" timeout:"<<in->timeout;
            out->result = standby(cmd, in->timeout);
            break;
        case OP_POWERON:
            DBG<<"Poweron "<<" timeout:"<<in->timeout;
            out->result = poweron(cmd, in->timeout);
            break;
        case OP_GET_STATE:{
            std::string desc;
            out->result = getState(cmd, &out->ivalue,desc,in->timeout);
            strncpy(out->str,desc.c_str(),MAX_STR_SIZE);
            DBG<<"Got State: "<<out->ivalue<<" \""<<desc<<"\" timeout:"<<in->timeout;
            break;
        }
        case OP_GET_SWVERSION:{
            std::string ver;
            out->result = getSWVersion(cmd, ver,in->timeout);
            DBG<<"Got HW Version:\""<<ver<<"\" timeout:"<<in->timeout;
            strncpy(out->str,ver.c_str(),MAX_STR_SIZE);;
            break;
        }
        case OP_GET_HWVERSION:{
            std::string ver;
            out->result = getHWVersion(cmd, ver,in->timeout);
            DBG<<"Got SW Version:\""<<ver<<"\" timeout:"<<in->timeout;
            strncpy(out->str,ver.c_str(),MAX_STR_SIZE);;
            break;
        }
        case OP_GET_CURRENT_SENSIBILITY:
            out->result = getCurrentSensibility(cmd, &out->fvalue0);
            DBG<<"Got Current sensibility: \""<<out->fvalue0<<"\"";
            break;
        case OP_GET_VOLTAGE_SENSIBILITY:
            out->result = getVoltageSensibility(cmd, &out->fvalue0);
            DBG<<"Got Voltage sensibility: \""<<out->fvalue0<<"\"";
            break;
            
        case OP_GET_MAXMIN_CURRENT:
            out->result = getMaxMinCurrent(cmd, &out->fvalue0,&out->fvalue1);
            DBG<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" current";
            
            break;
        case OP_GET_MAXMIN_VOLTAGE:
            out->result = getMaxMinVoltage(cmd, &out->fvalue0,&out->fvalue1);
            DBG<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" voltage";
            break;
        case OP_GET_ALARM_DESC:
            out->result = getAlarmDesc(cmd, &out->alarm_mask);
            DBG<<"Got Alarm maxk "<<out->alarm_mask;
            break;
        default:
            ERR<<"Opcode not supported:"<<cmd->opcode;
    }
    return result;
}
