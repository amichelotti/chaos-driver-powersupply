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
#include "ChaosPowerSupplyExtDD.h"

#include <string>
#include <boost/regex.hpp>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include "ChaosPowerSupplyExtDD.h"
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>
using namespace chaos::driver::powersupply;
//default constructor definition
ChaosPowerSupplyExtDD::ChaosPowerSupplyExtDD() {
	
}


/*
 *	ChaosPowerSupplyExtDD.cpp
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
#undef DEBUG
#include "ChaosPowerSupplyExtDD.h"
#include "../../core/ChaosPowerSupplyInterface.h"

#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataVariant.h>
#undef INFO
#undef ERR
#undef DBG
#define INFO INFO_LOG(ChaosPowerSupplyExtDD)
#define ERR ERR_LOG(ChaosPowerSupplyExtDD)
#define DBG DBG_LOG(ChaosPowerSupplyExtDD)

using namespace chaos::driver::powersupply;
using namespace chaos::common::data;
using namespace chaos::cu::driver_manager::driver;
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(ChaosPowerSupplyExtDD, 1.0.0, chaos::driver::powersupply::ChaosPowerSupplyExtDD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::powersupply::ChaosPowerSupplyExtDD, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::powersupply::ChaosPowerSupplyExtDD)
CLOSE_REGISTER_PLUGIN


ChaosPowerSupplyExtDD::~ChaosPowerSupplyExtDD() {}

void ChaosPowerSupplyExtDD::driverInit(const chaos::common::data::CDataWrapper& init_parameter)  {
    INFO << init_parameter.getJSONString();
    client.driverInit(init_parameter);

}
void ChaosPowerSupplyExtDD::driverInit(const char*c) {

}

void ChaosPowerSupplyExtDD::driverDeinit()  {
        client.driverDeinit();

}

int ChaosPowerSupplyExtDD::asyncMessageReceived(CDWUniquePtr message) {
    INFO << message->getJSONString();
    return 0;
}

#define WRITE_ERR_ON_CMD(r, c, m, d)\
ERR <<m<<" domain:"<<d;

#define RETURN_ERROR(r, c, m, d)\
WRITE_ERR_ON_CMD(r, c, m, d)\
return err;

#define SEND_REQUEST(c, r,a)\
int err;\
if(err=client.sendRawRequest(ChaosMoveOperator(r), a)) {\
WRITE_ERR_ON_CMD(c, -1, "Timeout waiting answer from remote driver", __PRETTY_FUNCTION__);\
} else {\
if(response->hasKey("err")) {\
if(response->isInt32Value("err") == false)  {\
WRITE_ERR_ON_CMD(c, -3, "'err' key need to be an int32 value", __PRETTY_FUNCTION__);\
} else {\
err = response->getInt32Value("err");\
}\
} else {\
WRITE_ERR_ON_CMD(c, -2, "'err' key not found on external driver return package", __PRETTY_FUNCTION__);\
}\
}

#define SEND_REQUEST_OPC(opc, r,a) \
int err;{\
if((err=client.sendOpcodeRequest(opc,ChaosMoveOperator(r),a))) {\
WRITE_ERR_ON_CMD(err, -1, "Error from from remote driver", __PRETTY_FUNCTION__);\
}else {\
    if(response.get()==NULL){ERR<<"EMPTY RESPONSE";return -1;}\
    if(response->hasKey("err")) {\
    if(response->isInt32Value("err") == false)  {\
    WRITE_ERR_ON_CMD(c, -3, "'err' key need to be an int32 value", __PRETTY_FUNCTION__);\
    } else {\
    err = response->getInt32Value("err");\
    }\
    } else {\
    WRITE_ERR_ON_CMD(c, -2, "'err' key not found on external driver return package", __PRETTY_FUNCTION__);\
    }}\
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

int ChaosPowerSupplyExtDD::initPS() {
    CDWShrdPtr response;

    CDWUniquePtr init_pack(new CDataWrapper());

    SEND_REQUEST_OPC("init", init_pack, response);

    return err;
}

int ChaosPowerSupplyExtDD::deinitPS() {
    CDWShrdPtr response;

    CDWUniquePtr init_pack(new CDataWrapper());

    SEND_REQUEST_OPC("deinit", init_pack, response);


    return err;
}

int ChaosPowerSupplyExtDD::setPolarity( int pol, uint32_t timeo_ms) {
    CDWShrdPtr response;
    CDWUniquePtr set_pola_pack(new CDataWrapper());
    set_pola_pack->addInt32Value("value", pol);

    SEND_REQUEST_OPC("set_pola", set_pola_pack, response);
  
    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::getPolarity(int* pol,int*polsp,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    SEND_REQUEST_OPC("get_pola", init_pack, response);
   // CHECK_KEY_AND_TYPE_IN_RESPONSE(response, "value", isInt32Value, -1, -2);
    *pol = response->getInt32Value("value");
    if(polsp){
        //TODO: CHANGE IF SUPPORTED
        *polsp=*pol;
    }
    return response->getInt32Value("err");
}

int ChaosPowerSupplyExtDD::setCurrentSP( float current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr set_cur_pack(new CDataWrapper());
   // init_pack->addStringValue("opc", "set_cur");
    set_cur_pack->addDoubleValue("value", current);
  
  SEND_REQUEST_OPC("set_cur", set_cur_pack, response);
  return response->getInt32Value("err");
}

int ChaosPowerSupplyExtDD::getCurrentSP(float* current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    //init_pack->addStringValue("opc", "get_cur");
   // SEND_REQUEST(cmd, init_pack, response);
    SEND_REQUEST_OPC("getSP", init_pack, response);
    //CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *current = response->getDoubleValue("value");
    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::startCurrentRamp(uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    SEND_REQUEST_OPC("start_ramp", init_pack, response);

  //  if(response.get()){DBG << response->getJSONString();}
    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::getVoltageOutput(float* volt,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    SEND_REQUEST_OPC("get_vol", init_pack, response);

    
    //CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *volt = response->getDoubleValue("value");
    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::getCurrentOutput(float* current,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    SEND_REQUEST_OPC("get_cur", init_pack, response);

    //CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *current = response->getDoubleValue("value");
    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::setCurrentRampSpeed(float asup, float asdown, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    init_pack->addDoubleValue("up", asup);
    init_pack->addDoubleValue("down", asdown);
    SEND_REQUEST_OPC("set_slope", init_pack, response);

    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::resetAlarms( uint64_t alrm,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    SEND_REQUEST_OPC("rst_alarm", init_pack, response);

    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::getAlarms(uint64_t*alrm,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    SEND_REQUEST_OPC("get_alarm", init_pack, response);

    
    //CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *alrm = response->getInt32Value("value");
    return response->getInt32Value("err");;
}

int ChaosPowerSupplyExtDD::shutdown(uint32_t timeo_ms){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::standby(uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"SET STANDBY...";

    init_pack->addInt32Value("value",0);
    SEND_REQUEST_OPC("set_mode", init_pack, response);
    
    return err;


}

int ChaosPowerSupplyExtDD::poweron(uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"SET OPER...";

    init_pack->addInt32Value("value",1);
    SEND_REQUEST_OPC("set_mode", init_pack, response);
   
    return err;

}

int ChaosPowerSupplyExtDD::getState(int* state,std::string& desc,int*statesp,uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"GETTING STATE...";
    //init_pack->addStringValue(MESSAGE_URI,remote_uri_instance);
    SEND_REQUEST_OPC("get_state", init_pack, response);
    if(response.get()){DBG << response->getJSONString();}
    
    //CHECK_KEY_IN_RESPONSE(response, "value", -1);
    *state = response->getInt32Value("value");
    if(statesp){
        // TODO: if returned set the state SP
            *statesp = response->getInt32Value("value");

    }
    if(response->hasKey("description")){
        desc = response->getStringValue("description");

    }
    return err;
}

int ChaosPowerSupplyExtDD::getSWVersion(std::string& version,uint32_t timeo_ms){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::getHWVersion(std::string& version, uint32_t timeo_ms){
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
    DBG<<"GETTING VERSION...";
    SEND_REQUEST_OPC("get_hw_ver", init_pack, response);

    if(response->hasKey("value")){
        version = response->getStringValue("value");
    }
    return response->getInt32Value("err");
}

int ChaosPowerSupplyExtDD::getCurrentSensibility(float *sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::getVoltageSensibility(float *sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::setCurrentSensibility(float sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::setVoltageSensibility(float sens){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::getMaxMinCurrent(float*max,float*min){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::getMaxMinVoltage(float*max,float*min){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::getAlarmDesc(uint64_t* alarm){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::forceMaxCurrent(float max){
    CHAOS_ASSERT(false);
    return 0;
}

int ChaosPowerSupplyExtDD::forceMaxVoltage(float max){
    CHAOS_ASSERT(false);
    return 0;
}

uint64_t ChaosPowerSupplyExtDD::getFeatures() {
    CDWShrdPtr response;
    CDWUniquePtr init_pack(new CDataWrapper());
   // init_pack->addStringValue("opc", "get_feature");
    SEND_REQUEST_OPC("get_feature", init_pack, response);
    
    //CHECK_KEY_IN_RESPONSE(response, "value", -1);
    return response->getInt32Value("value");
}
            
        
//! Execute a command
MsgManagmentResultType::MsgManagmentResult ChaosPowerSupplyExtDD::execOpcode(DrvMsgPtr cmd) {
         ERR<<"Opcode not supported:"<<cmd->opcode;
         return MsgManagmentResultType::MMR_ERROR				/**< Opcode operation ha given an error. */
;
    
}
