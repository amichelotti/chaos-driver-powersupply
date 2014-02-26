/*
 *	Generic Power Supply
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
#include "GenericPowerSupplyDD.h"

#include <string>
#include <boost/regex.hpp>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include "driver/powersupply/core/ChaosPowerSupplyInterface.h"

#define PSLAPP		LAPP_ << "[GenericPowerSupply] "
#define PSDBG		LDBG_ << "[GenericPowerSupply] "
#define PSERR		LERR_ << "[GenericPowerSupply] "

// initialization format is <POWERSUPPLY TYPE>:'<INITALISATION PARAMETERS>'
static const boost::regex power_supply_init_match("(\\w+):(.+)");

// initialisation format for ocem <serial port>,<slaveid>,<maxcurr:maxvoltage>
static const boost::regex power_supply_ocem_init_match("([\\w\\/]+),(\\d+),(\\d+):(\\d+)");

// initialisation format for simulator <serial port>,<slaveid>,<write_latency_min:write_latency_max>,<read_latency_min:read_latency_min>,<maxcurr:max voltage>

static const boost::regex power_supply_simulator_init_match("([\\w\\/]+),(\\d+),(\\d+):(\\d+),(\\d+):(\\d+),(\\d+):(\\d+)");


//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(GenericPowerSupplyDD, 1.0.0, chaos_powersupply_dd::GenericPowerSupplyDD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(driver::powersupply::GenericPowerSupplyDD, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(driver::powersupply::GenericPowerSupplyDD)
CLOSE_REGISTER_PLUGIN


//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR_WITH_NS(chaos_powersupply_dd, GenericPowerSupplyDD) {
    power = NULL;
	
}

//default descrutcor
chaos_powersupply_dd::GenericPowerSupplyDD::~GenericPowerSupplyDD() {
	
}

void chaos_powersupply_dd::GenericPowerSupplyDD::driverInit(const char *initParameter) throw(chaos::CException) {
    //check the input parameter
	boost::smatch match;
	std::string inputStr = initParameter;
	PSLAPP << "Init GenericPowerSupply driver initialisation string:\""<<initParameter<<"\""<<endl;
    if(power){
          throw chaos::CException(1, "Already Initialised", "GenericPowerSupplyDD::driverInit");
    }
    if(regex_match(inputStr, match, power_supply_init_match, boost::match_extra)){
        std::string powerSupplyType=match[1];
        std::string initString=match[2];
        if(powerSupplyType=="OcemE642X"){
            if(regex_match(initString, match, power_supply_ocem_init_match, boost::match_extra)){
                std::string dev=match[1];
                std::string slaveid=match[2];
                std::string maxcurr=match[3];
                std::string maxvoltage=match[3];
                PSLAPP<<"Allocating OcemE642X device \""<<slaveid<<"\""<<" on dev:\""<<dev<<"\""<<endl;
                power = new ::common::powersupply::OcemE642X(dev.c_str(),atoi(slaveid.c_str()),atof(maxcurr.c_str()),atof(maxvoltage.c_str()));
                if(power==NULL){
                      throw chaos::CException(1, "Cannot allocate resources for OcemE642X", "GenericPowerSupplyDD::driverInit");
                }
            }
        } else if(powerSupplyType=="SimPSupply"){
            if(regex_match(initString, match, power_supply_simulator_init_match, boost::match_extra)){
                std::string dev=match[1];
                std::string slaveid=match[2];
                std::string write_min=match[3];
                std::string write_max=match[4];
                std::string read_min=match[5];
                std::string read_max=match[6];
                std::string max_curr=match[7];
                std::string max_vol=match[8];
                PSLAPP<<"Allocating Simulated Power Supply device \""<<slaveid<<"\""<<" on dev:\""<<dev<<"\""<<endl;
                power = new ::common::powersupply::SimPSupply(dev.c_str(),atoi(slaveid.c_str()),atoi(write_min.c_str()),atoi(write_max.c_str()),atoi(read_min.c_str()),atoi(read_max.c_str()),atoi(max_curr.c_str()),atoi(max_vol.c_str()));
                if(power==NULL){
                    throw chaos::CException(1, "Cannot allocate resources for SimPSupply", "GenericPowerSupplyDD::driverInit");
                }
            } else {
                throw chaos::CException(1, "Bad parameters for SimSupply", "GenericPowerSupplyDD::driverInit");

            }
        } else {
              throw chaos::CException(1, "Unsupported Power Supply", "GenericPowerSupplyDD::driverInit");
        }
    } else {
        throw chaos::CException(1, "Malformed initialisation string", "GenericPowerSupplyDD::driverInit");

    }
    std::string ver;
    power->getSWVersion(ver,0);
    PSLAPP<<"Initialising PowerSupply Driver \""<<ver<<"\""<<endl;

    if(power->init()!=0){
        throw chaos::CException(1, "Initialisation of power supply failed", "GenericPowerSupplyDD::driverInit");
    }

    
}

void chaos_powersupply_dd::GenericPowerSupplyDD::driverDeinit() throw(chaos::CException) {
	PSLAPP << "Deinit GenericPowerSupply driver";
    if(power){
        delete power;
    }
    power = NULL;
}


cu_driver::MsgManagmentResultType::MsgManagmentResult chaos_powersupply_dd::GenericPowerSupplyDD::execOpcode(cu_driver::DrvMsgPtr cmd){
    cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
    chaos_powersupply_dd::powersupply_iparams_t *in = (chaos_powersupply_dd::powersupply_iparams_t *)cmd->inputData;
    chaos_powersupply_dd::powersupply_oparams_t *out = (chaos_powersupply_dd::powersupply_oparams_t *)cmd->resultData;

    switch(cmd->opcode){
            
        case OP_SET_POLARITY:
            LDBG_<< "Set Polarity to:"<<in->ivalue<<" timeo:"<<in->timeout<<endl;
            out->result= power->setPolarity(in->ivalue,in->timeout);
            
            break;
        case OP_GET_POLARITY:
            out->result = power->getPolarity(&out->ivalue,in->timeout);
            LDBG_<< "Got Polarity "<<out->ivalue<<endl;
            break;
        case OP_SET_SP:
            LDBG_<< "Set Current SP to:"<<in->fvalue0<<" timeo:"<<in->timeout<<endl;
            out->result = power->setCurrentSP(in->fvalue0,in->timeout);
            break;
        case OP_GET_SP: // get current set point
            out->result = power->getCurrentSP(&out->fvalue0,in->timeout);
            LDBG_<< "Got Current SP "<<out->fvalue0<<endl;
            break;
        case OP_START_RAMP: // start ramp
            LDBG_<< "Start Ramp timeo:"<<in->timeout<<endl;
            out->result = power->startCurrentRamp(in->timeout);
            break;
        case OP_GET_VOLTAGE_OUTPUT:
            out->result = power->getVoltageOutput(&out->fvalue0,in->timeout);
            LDBG_<< "Got Voltage "<<out->fvalue0<<endl;
            break;
        case OP_GET_CURRENT_OUTPUT:
            out->result = power->getCurrentOutput(&out->fvalue0,in->timeout);
            LDBG_<< "Got Current "<<out->fvalue0<<endl;
            break;
        case OP_SET_CURRENT_RAMP_SPEED:
            LDBG_<<"Setting current ramp speed min:"<<in->fvalue0<<" max:"<<in->fvalue1<<" timeout:"<<in->timeout<<endl;
            out->result = power->setCurrentRampSpeed(in->fvalue0,in->fvalue1,in->timeout);
            break;
        case OP_RESET_ALARMS:
            LDBG_<<"Reset alarms to:"<<in->alarm_mask<<" timeout:"<<in->timeout<<endl;
            out->result = power->resetAlarms(in->alarm_mask,in->timeout);
            break;
        case OP_GET_ALARMS:
            out->result = power->getAlarms(&out->alarm_mask,in->timeout);
            LDBG_<<"Got alarms to:"<<out->alarm_mask<<" timeout:"<<in->timeout<<endl;
            break;
        case OP_SHUTDOWN:
            LDBG_<<"Shutting down"<<" timeout:"<<in->timeout<<endl;
            out->result = power->shutdown(in->timeout);
            break;
        case OP_STANDBY:
            LDBG_<<"Standby "<<" timeout:"<<in->timeout<<endl;
            out->result = power->standby(in->timeout);
            break;
        case OP_POWERON:
            LDBG_<<"Poweron "<<" timeout:"<<in->timeout<<endl;
            out->result = power->poweron(in->timeout);
            break;
        case OP_GET_STATE:{
            std::string desc;
            out->result = power->getState(&out->ivalue,desc,in->timeout);
            strncpy(out->str,desc.c_str(),MAX_STR_SIZE);
            LDBG_<<"Got State: "<<out->ivalue<<" \""<<desc<<"\" timeout:"<<in->timeout<<endl;
            break;
        }
        case OP_GET_SWVERSION:{
            std::string ver;
            out->result = power->getSWVersion(ver,in->timeout);
            LDBG_<<"Got HW Version:\""<<ver<<"\" timeout:"<<in->timeout<<endl;
            break;
        }
        case OP_GET_HWVERSION:{
            std::string ver;
            out->result = power->getHWVersion(ver,in->timeout);
            LDBG_<<"Got SW Version:\""<<ver<<"\" timeout:"<<in->timeout<<endl;
            break;
        }
        case OP_GET_CURRENT_SENSIBILITY:
            out->result = power->getCurrentSensibility(&out->fvalue0);
            LDBG_<<"Got Current sensibility: \""<<out->fvalue0<<"\""<<endl;
            break;
        case OP_GET_VOLTAGE_SENSIBILITY:
            out->result = power->getVoltageSensibility(&out->fvalue0);
            LDBG_<<"Got Voltage sensibility: \""<<out->fvalue0<<"\""<<endl;
            break;
        
        case OP_GET_MAXMIN_CURRENT:
            out->result = power->getMaxMinCurrent(&out->fvalue0,&out->fvalue1);
            LDBG_<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" current"<<endl;

            break;
        case OP_GET_MAXMIN_VOLTAGE:
            out->result = power->getMaxMinVoltage(&out->fvalue0,&out->fvalue1);
            LDBG_<<"Got Max "<<out->fvalue0<<" Min "<< out->fvalue1<<" voltage"<<endl;
            break;
        case OP_GET_ALARM_DESC:
            out->result = power->getAlarmDesc(&out->alarm_mask);
            LDBG_<<"Got Alarm maxk "<<out->alarm_mask<<endl;

            break;

    }
    return result;
}





