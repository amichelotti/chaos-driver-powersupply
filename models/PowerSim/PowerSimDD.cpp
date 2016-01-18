/*
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
#include "PowerSimDD.h"

#include <string>
#include <boost/regex.hpp>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include "driver/powersupply/core/ChaosPowerSupplyInterface.h"


// initialization format is <POWERSUPPLY TYPE>:'<INITALISATION PARAMETERS>'
static const boost::regex power_supply_init_match("(\\w+):(.+)");

// initialisation format for simulator <serial port>,<slaveid>,<write_latency_min:write_latency_max>,<read_latency_min:read_latency_min>,<maxcurr:max voltage>,<force errors secs=0 [no error]>

static const boost::regex power_supply_simulator_init_match("([\\w\\/]+),(\\d+),(\\d+):(\\d+),(\\d+):(\\d+),(\\d+):(\\d+),(\\d+)");


//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(PowerSimDD, 1.0.0, chaos::driver::powersupply::PowerSimDD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::powersupply::PowerSimDD, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::powersupply::PowerSimDD)
CLOSE_REGISTER_PLUGIN


//default constructor definition
chaos::driver::powersupply::PowerSimDD::PowerSimDD() {
    power = NULL;
	
}

//default descrutcor
chaos::driver::powersupply::PowerSimDD::~PowerSimDD() {
	
}

void chaos::driver::powersupply::PowerSimDD::driverInit(const char *initParameter) throw(chaos::CException) {
    //check the input parameter
	boost::smatch match;
	std::string inputStr = initParameter;
	PSLAPP << "Init PowerSimDD driver initialisation string:\""<<initParameter<<"\""<<std::endl;
    if(power){
          throw chaos::CException(1, "Already Initialised", "PowerSimDD::driverInit");
    }
    if(regex_match(inputStr, match, power_supply_init_match, boost::match_extra)){
        std::string powerSupplyType=match[1];
        std::string initString=match[2];
        if(powerSupplyType=="SimPSupply"){
            if(regex_match(initString, match, power_supply_simulator_init_match, boost::match_extra)){
                std::string dev=match[1];
                std::string slaveid=match[2];
                std::string write_min=match[3];
                std::string write_max=match[4];
                std::string read_min=match[5];
                std::string read_max=match[6];
                std::string max_curr=match[7];
                std::string max_vol=match[8];
                std::string force_err=match[9];
                PSLAPP<<"Allocating Simulated Power Supply device \""<<slaveid<<"\""<<" on dev:\""<<dev<<"\" FORCING ERRORS:"<<force_err<<std::endl;
                power = new ::common::powersupply::SimPSupply(dev.c_str(),atoi(slaveid.c_str()),atoi(write_min.c_str()),atoi(write_max.c_str()),atoi(read_min.c_str()),atoi(read_max.c_str()),atoi(max_curr.c_str()),atoi(max_vol.c_str()),SIMPSUPPLY_UPDATE_DELAY,atoi(force_err.c_str()));
                if(power==NULL){
                    throw chaos::CException(1, "Cannot allocate resources for SimPSupply", "PowerSimDD::driverInit");
                }
            } else {
                throw chaos::CException(1, "Bad parameters for PowerSimDD <serial port>,<slaveid>,<write_latency_min:write_latency_max>,<read_latency_min:read_latency_min>,<maxcurr:max voltage>", "PowerSimDD::driverInit");

            }
        } else {
              throw chaos::CException(1, "Unsupported Power Supply", "PowerSimDD::driverInit");
        }
    } else {
        throw chaos::CException(1, "Malformed initialisation string", "PowerSimDD::driverInit");

    }
    std::string ver;
    power->getSWVersion(ver,0);
    PSLAPP<<"Initialising PowerSimDD Driver \""<<ver<<"\""<<std::endl;

    if(power->init()!=0){
        throw chaos::CException(1, "Initialisation of power supply failed", "PowerSimDD::driverInit");
    }

    
}
