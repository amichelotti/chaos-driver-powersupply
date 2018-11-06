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
#include <chaos/common/data/CDataVariant.h>
#undef INFO
#undef ERR
#undef DBG
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

#define SEND_REQUEST_OPC(opc,c, r,a) {\
    int err;\
try { \
if(err=sendOpcodeRequest(opc,ChaosMoveOperator(r),a)) {\
WRITE_ERR_ON_CMD(err, -1, "Error from from remote driver", __PRETTY_FUNCTION__);\
}else {\
    if(response->hasKey("err")) {\
    if(response->isInt32Value("err") == false)  {\
    WRITE_ERR_ON_CMD(c, -3, "'err' key need to be an int32 value", __PRETTY_FUNCTION__);\
    } else {\
    c->ret = response->getInt32Value("err");\
    }\
    } else {\
    WRITE_ERR_ON_CMD(c, -2, "'err' key not found on external driver return package", __PRETTY_FUNCTION__);\
    }}\
	}\
    catch (...) { \
WRITE_ERR_ON_CMD(err, -6, "Exception from from remote driver", __PRETTY_FUNCTION__);\
} \
}

#define CHECK_KEY_AND_TYPE_IN_RESPONSE(r, k, t, e1, e2)\
if(!r->hasKey(k)) {\
std::string es1 = CHAOS_FORMAT("'%1%' key is mandatory in remote driver response",%k);\
RETURN_ERROR(cmd, e1, es1.c_str(), __PRETTY_FUNCTION__);\
} else if(!r->t(k)) {\
std::string es2 = CHAOS_FORMAT("'%1%' key in remote driver response need to be int32 value", %k);\
RETURN_ERROR(cmd, e2, es2.c_str(), __PRETTY_FUNCTION__);\
}

#define CHECK_KEY_IN_RESPONSE(r, k, e1)\
if(!r->hasKey(k)) {\
std::string es1 = CHAOS_FORMAT("'%1%' key is mandatory in remote driver response",%k);\
RETURN_ERROR(cmd, e1, es1.c_str(), __PRETTY_FUNCTION__);\
}

int ChaosPowerSupplyOpcodeLogic::sendInit(DrvMsgPtr cmd) {
    CDWShrdPtr response;

    CDWUniquePtr init_pack(new CDataWrapper());

    cmd->ret =  0;
    SEND_REQUEST_OPC("init",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}

    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::sendDeinit(DrvMsgPtr cmd) {
    CDWShrdPtr response;

    CDWUniquePtr init_pack(new CDataWrapper());

    cmd->ret =  0;
    SEND_REQUEST_OPC("deinit",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}

    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::setPolarity(DrvMsgPtr cmd, int pol, uint32_t timeo_ms) {
    CDWShrdPtr response;
    CDWUniquePtr set_pola_pack(new CDataWrapper());
    set_pola_pack->addInt32Value("value", pol);

    SEND_REQUEST_OPC("set_pola",cmd, set_pola_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getPolarity(DrvMsgPtr cmd, int* pol,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    SEND_REQUEST_OPC("get_pola",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_AND_TYPE_IN_RESPONSE(response, "value", isInt32Value, -1, -2);
    *pol = response->getInt32Value("value");
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentSP(DrvMsgPtr cmd, float current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr set_cur_pack(new CDataWrapper());
   // init_pack->addStringValue("opc", "set_cur");
    set_cur_pack->addDoubleValue("value", current);
   // SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("set_cur",cmd, set_cur_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentSP(DrvMsgPtr cmd, float* current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    //init_pack->addStringValue("opc", "get_cur");
   // SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("getSP",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *current = response->getVariantValue("value").asDouble();
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::startCurrentRamp(DrvMsgPtr cmd, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
  //  init_pack->addStringValue("opc", "start_ramp");
   // SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("start_ramp",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getVoltageOutput(DrvMsgPtr cmd, float* volt,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    //init_pack->addStringValue("opc", "get_vol");
   // SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("get_vol",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *volt = response->getVariantValue("value").asDouble();
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentOutput(DrvMsgPtr cmd, float* current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
   // init_pack->addStringValue("opc", "get_cur");
   // SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("get_cur",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    //CDataWrapperType type = response->getValueType("value");
   // INFO << type;
    CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *current = response->getVariantValue("value").asDouble();
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentRampSpeed(DrvMsgPtr cmd, float asup, float asdown, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addDoubleValue("up", asup);
    init_pack->addDoubleValue("down", asdown);
    SEND_REQUEST_OPC("set_slope",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::resetAlarms(DrvMsgPtr cmd, uint64_t alrm,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    //CDWUniquePtr set_slope_pack(new CDataWrapper());
   // init_pack->addStringValue("opc", "rst_alarm");
   // SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("rst_alarm",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getAlarms(DrvMsgPtr cmd, uint64_t*alrm,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
   // init_pack->addStringValue("opc", "get_alarm");
  //  SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("get_alarm",cmd, init_pack, response);

    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *alrm = response->getVariantValue("value").asUInt64();
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::shutdown(DrvMsgPtr cmd, uint32_t timeo_ms){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::standby(DrvMsgPtr cmd, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"SET STANDBY...";

    init_pack->addInt32Value("value",0);
    SEND_REQUEST_OPC("set_mode",cmd, init_pack, response);
    if(response.get()){DBG << response->getJSONString();}
	else {DBG << "standby Response Null";}
    if(cmd->ret) {
        return cmd->ret;
    }
    return 0;


}

int ChaosPowerSupplyOpcodeLogic::poweron(DrvMsgPtr cmd, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"SET OPER...";

    init_pack->addInt32Value("value",1);
    SEND_REQUEST_OPC("set_mode",cmd, init_pack, response);
    if(response.get()){DBG << response->getJSONString();}
	else {DBG << "standby Response Null";}
    if(cmd->ret) {
        return cmd->ret;
    }
    return 0;

}

int ChaosPowerSupplyOpcodeLogic::getState(DrvMsgPtr cmd, int* state,std::string& desc,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"GETTING STATE...";
    //init_pack->addStringValue(MESSAGE_URI,remote_uri_instance);
    SEND_REQUEST_OPC("get_state",cmd, init_pack, response);
    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", -1);
    CHECK_KEY_IN_RESPONSE(response, "description", -2);
    *state = response->getVariantValue("value").asInt32();
    desc = response->getVariantValue("description").asString();
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getSWVersion(DrvMsgPtr cmd, std::string& version,uint32_t timeo_ms){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getHWVersion(DrvMsgPtr cmd, std::string& version, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"GETTING VERSION...";
    //init_pack->addStringValue("opc", "get_hw_ver");
    SEND_REQUEST_OPC("get_hw_ver",cmd, init_pack, response);

    //SEND_REQUEST(cmd, init_pack, response);
    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", -1);
    version = response->getVariantValue("value").asString();
    return cmd->ret;
}

int ChaosPowerSupplyOpcodeLogic::getCurrentSensibility(DrvMsgPtr cmd, float *sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getVoltageSensibility(DrvMsgPtr cmd, float *sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::setCurrentSensibility(DrvMsgPtr cmd, float sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::setVoltageSensibility(DrvMsgPtr cmd, float sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getMaxMinCurrent(DrvMsgPtr cmd, float*max,float*min){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getMaxMinVoltage(DrvMsgPtr cmd, float*max,float*min){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::getAlarmDesc(DrvMsgPtr cmd, uint64_t* alarm){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::forceMaxCurrent(DrvMsgPtr cmd, float max){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyOpcodeLogic::forceMaxVoltage(DrvMsgPtr cmd, float max){
    CHAOS_ASSERT(false);
    return 0;
}

uint64_t ChaosPowerSupplyOpcodeLogic::getFeatures(DrvMsgPtr cmd) {
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
   // init_pack->addStringValue("opc", "get_feature");
    SEND_REQUEST_OPC("get_feature",cmd, init_pack, response);
    if(response.get()){DBG << response->getJSONString();}
    if(cmd->ret) {return cmd->ret;}
    
    CHECK_KEY_IN_RESPONSE(response, "value", -1);
    return response->getVariantValue("value").asUInt64();
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
        case OP_INIT:
            out->result = sendInit(cmd);
            break;
        case OP_DEINIT:
            out->result = sendDeinit(cmd);
            break;
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
