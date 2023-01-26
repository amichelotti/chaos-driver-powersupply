/*
 *	Hazemeyer AL250 Device Driver
 *	!CHAOS
 *	Created by Alessandro D'Uffizi
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
#include "ChaosAL250.h"

#include <string>
#include <regex>
#include <common/modbus/core/ModbusChannelFactory.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

// initialization format is <POWERSUPPLY TYPE>:'<INITALISATION PARAMETERS>'
static const std::regex power_supply_init_match("(\\w+):(.+)");

// initialisation format for ocem <serialstring>:<slaveid>
static const std::regex power_supply_hazemeyer_init_match("([\\w\\/,]+):(\\d+)");

//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are making a plugin

OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(C_AL250, 1.0.0, chaos::driver::powersupply::C_AL250)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::powersupply::C_AL250, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::powersupply::C_AL250)
CLOSE_REGISTER_PLUGIN


//default constructor definition
chaos::driver::powersupply::C_AL250::C_AL250() {
    power = NULL;
   
	
}

//default descrutcor
chaos::driver::powersupply::C_AL250::~C_AL250() {
	
}


void chaos::driver::powersupply::C_AL250::driverInit(const chaos::common::data::CDataWrapper& json) 
{
    PSLAPP << "Init  driver initialisation with json " <<json.getJSONString().c_str();
    ::common::modbus::AbstractModbusChannel_psh channel=::common::modbus::ModbusChannelFactory::getChannel(json);
    if(power)
    {
          throw chaos::CException(1, "Already Initialised", "C_AL250::driverInit");
    }
        
     power = new ::common::powersupply::AL250(channel,json);
     if(power==NULL)
     {
         throw chaos::CException(1, "Cannot allocate resources for C_AL250", "C_AL250::driverInit");
     }
    

    
}


void chaos::driver::powersupply::C_AL250::driverInit(const char *initParameter)  {
    //check the input parameter
	std::smatch match;
	std::string inputStr = initParameter;
	PSLAPP << "Init  driver initialisation string:\""<<initParameter<<"\""<<std::endl;
    if(power){
          throw chaos::CException(1, "Already Initialised", "C_AL250::driverInit");
    }
    if(std::regex_match(inputStr, match, power_supply_init_match, std::regex_constants::match_any)){
        std::string powerSupplyType=match[1];
        std::string initString=match[2];
        if(powerSupplyType=="HazemeyerAL250"){
            if(regex_match(initString, match, power_supply_hazemeyer_init_match, std::regex_constants::match_any)){
                std::string dev=match[1];
                std::string slaveid=match[2];
                PSLAPP<<"Allocating HazemeyerAL250 device \""<<slaveid<<"\""<<" on dev:\""<<dev<<"\""<<std::endl;
                power = new ::common::powersupply::AL250(dev.c_str(),atoi(slaveid.c_str()));
                if(power==NULL){
                      throw chaos::CException(1, "Cannot allocate resources for C_AL250", "C_AL250::driverInit");
                }
            
            } else {
                 throw chaos::CException(1, "Bad parameters for C_AL250 <serial port>,<slaveid>,<maxcurr:maxvoltage>", "OcemDD::driverInit");

            }
        } else {
            throw chaos::CException(1, "Unsupported driver", "C_AL250::driverInit");
        
        }
    } else {
            throw chaos::CException(1, "Malformed initialisation string", "C_AL250::driverInit");
    
    }
    
    std::string ver;
    power->getSWVersion(ver,0);
    PSLAPP<<"Initialising PowerSupply Driver \""<<ver<<"\""<<std::endl;
/*
    if(power->init()!=0){
        throw chaos::CException(1, "Initialisation of power supply failed", "C_AL250::driverInit");
    }
    PSDBG<<"Init done";
*/
    
}

