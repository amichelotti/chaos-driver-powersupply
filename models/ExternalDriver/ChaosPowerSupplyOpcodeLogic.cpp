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

#define WRITE_ERR_ON_CMD(r ,c, m, d)\
cmd->ret = c;\
snprintf(cmd->err_msg, 255, m);\
snprintf(cmd->err_dom, 255, d);

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

//! Execute a command
MsgManagmentResultType::MsgManagmentResult ChaosPowerSupplyOpcodeLogic::execOpcode(DrvMsgPtr cmd) {
    MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
        case OP_INIT:{
            CDWShrdPtr response;
            CDWUniquePtr init_pack(new CDataWrapper());
            init_pack->addStringValue("opc", "init");
            init_pack->addCSDataValue("par", *powersupply_init_pack);
            SEND_REQUEST(cmd, init_pack, response);
            if(response.get()){INFO << response->getJSONString();}
            break;
        }
        case OP_DEINIT:{
            CDWShrdPtr response;
            CDWUniquePtr init_pack(new CDataWrapper());
            init_pack->addStringValue("opc", "deinit");
            init_pack->addCSDataValue("par", *powersupply_init_pack);
            SEND_REQUEST(cmd, init_pack, response);
            if(response.get()){INFO << response->getJSONString();}
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
