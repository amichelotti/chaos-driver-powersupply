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
#include "driver/powersupply/core/ChaosPowerSupplyInterface.h"

#define PSLAPP		LAPP_ << "[ChaosPowerSupplyDD] "
#define PSDBG		LDBG_ << "[ChaosPowerSupplyDD] "
#define PSERR		LERR_ << "[ChaosPowerSupplyDD] "

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
        delete power;
    }
    power = NULL;
}


cu_driver::MsgManagmentResultType::MsgManagmentResult ChaosPowerSupplyDD::execOpcode(cu_driver::DrvMsgPtr cmd){
    cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
    powersupply_iparams_t *in = (powersupply_iparams_t *)cmd->inputData;
    powersupply_oparams_t *out = (powersupply_oparams_t *)cmd->resultData;

    switch(cmd->opcode){
            
        case OP_SET_POLARITY:
            LDBG_<< "Set Polarity to:"<<in->ivalue<<" timeo:"<<in->timeout<<std::endl;
            out->result= power->setPolarity(in->ivalue,in->timeout);
            
            break;
        case OP_GET_POLARITY:
            out->result = power->getPolarity(&out->ivalue,in->timeout);
            LDBG_<< "Got Polarity "<<out->ivalue<<std::endl;
            break;
        case OP_SET_SP:
            LDBG_<< "Set Current SP to:"<<in->fvalue0<<" timeo:"<<in->timeout<<std::endl;
            out->result = power->setCurrentSP(in->fvalue0,in->timeout);
            break;
        case OP_GET_SP: // get current set point
            out->result = power->getCurrentSP(&out->fvalue0,in->timeout);
            LDBG_<< "Got Current SP "<<out->fvalue0<<std::endl;
            break;
        case OP_START_RAMP: // start ramp
            LDBG_<< "Start Ramp timeo:"<<in->timeout<<std::endl;
            out->result = power->startCurrentRamp(in->timeout);
            break;
        case OP_GET_VOLTAGE_OUTPUT:
            out->result = power->getVoltageOutput(&out->fvalue0,in->timeout);
            LDBG_<< "Got Voltage "<<out->fvalue0<<std::endl;
            break;
        case OP_GET_CURRENT_OUTPUT:
            out->result = power->getCurrentOutput(&out->fvalue0,in->timeout);
            LDBG_<< "Got Current "<<out->fvalue0<<std::endl;
            break;
        case OP_SET_CURRENT_RAMP_SPEED:
            LDBG_<<"Setting current ramp speed min:"<<in->fvalue0<<" max:"<<in->fvalue1<<" timeout:"<<in->timeout<<std::endl;
            out->result = power->setCurrentRampSpeed(in->fvalue0,in->fvalue1,in->timeout);
            break;
        case OP_RESET_ALARMS:
            LDBG_<<"Reset alarms to:"<<in->alarm_mask<<" timeout:"<<in->timeout<<std::endl;
            out->result = power->resetAlarms(in->alarm_mask,in->timeout);
            break;
        case OP_GET_ALARMS:
            out->result = power->getAlarms(&out->alarm_mask,in->timeout);
            LDBG_<<"Got alarms to:"<<out->alarm_mask<<" timeout:"<<in->timeout<<std::endl;
            break;
        case OP_SHUTDOWN:
            LDBG_<<"Shutting down"<<" timeout:"<<in->timeout<<std::endl;
            out->result = power->shutdown(in->timeout);
            break;
        case OP_STANDBY:
            LDBG_<<"Standby "<<" timeout:"<<in->timeout<<std::endl;
            out->result = power->standby(in->timeout);
            break;
        case OP_POWERON:
            LDBG_<<"Poweron "<<" timeout:"<<in->timeout<<std::endl;
            out->result = power->poweron(in->timeout);
            break;
        case OP_GET_STATE:{
            std::string desc;
            out->result = power->getState(&out->ivalue,desc,in->timeout);
            strncpy(out->str,desc.c_str(),MAX_STR_SIZE);
            LDBG_<<"Got State: "<<out->ivalue<<" \""<<desc<<"\" timeout:"<<in->timeout<<std::endl;
            break;
        }
        case OP_GET_SWVERSION:{
            std::string ver;
            out->result = power->getSWVersion(ver,in->timeout);
            LDBG_<<"Got HW Version:\""<<ver<<"\" timeout:"<<in->timeout<<std::endl;
            strncpy(out->str,ver.c_str(),MAX_STR_SIZE);;

            break;
        }
        case OP_GET_HWVERSION:{
            std::string ver;
            out->result = power->getHWVersion(ver,in->timeout);
            LDBG_<<"Got SW Version:\""<<ver<<"\" timeout:"<<in->timeout<<std::endl;
            strncpy(out->str,ver.c_str(),MAX_STR_SIZE);;

            break;
        }
        case OP_GET_CURRENT_SENSIBILITY:
            out->result = power->getCurrentSensibility(&out->fvalue0);
            LDBG_<<"Got Current sensibility: \""<<out->fvalue0<<"\""<<std::endl;
            break;
        case OP_GET_VOLTAGE_SENSIBILITY:
            out->result = power->getVoltageSensibility(&out->fvalue0);
            LDBG_<<"Got Voltage sensibility: \""<<out->fvalue0<<"\""<<std::endl;
            break;
        
        case OP_GET_MAXMIN_CURRENT:
            out->result = power->getMaxMinCurrent(&out->fvalue0,&out->fvalue1);
            LDBG_<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" current"<<std::endl;

            break;
        case OP_GET_MAXMIN_VOLTAGE:
            out->result = power->getMaxMinVoltage(&out->fvalue0,&out->fvalue1);
            LDBG_<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" voltage"<<std::endl;
            break;
        case OP_GET_ALARM_DESC:
            out->result = power->getAlarmDesc(&out->alarm_mask);
            LDBG_<<"Got Alarm maxk "<<out->alarm_mask<<std::endl;

            break;

    }
    return result;
}





