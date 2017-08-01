/*
 *	Ocem Device Driver
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
#include "OcemDD.h"

#include <string>
#include <boost/regex.hpp>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

// initialization format is <POWERSUPPLY TYPE>:'<INITALISATION PARAMETERS>'
static const boost::regex power_supply_init_match("(\\w+):(.+)");

// initialisation format for ocem <serial port>,<slaveid>,<maxcurr:maxvoltage>
static const boost::regex power_supply_ocem_init_match("([\\w\\/]+),(\\d+),(\\d+):(\\d+)");

//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(OcemDD, 1.0.0, chaos::driver::powersupply::OcemDD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::powersupply::OcemDD, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::powersupply::OcemDD)
CLOSE_REGISTER_PLUGIN


//default constructor definition
chaos::driver::powersupply::OcemDD::OcemDD() {
    power = NULL;
	
}

//default descrutcor
chaos::driver::powersupply::OcemDD::~OcemDD() {
	
}

void chaos::driver::powersupply::OcemDD::driverInit(const char *initParameter) throw(chaos::CException) {
    //check the input parameter
	boost::smatch match;
	std::string inputStr = initParameter;
        std::string slaveid;
	PSLAPP << "Init  driver initialisation string:\""<<initParameter<<"\""<<std::endl;
    if(power){
          throw chaos::CException(1, "Already Initialised", "OcemDD::driverInit");
    }
    if(regex_match(inputStr, match, power_supply_init_match, boost::match_extra)){
        std::string powerSupplyType=match[1];
        std::string initString=match[2];
        if(powerSupplyType=="OcemE642X"){
            if(regex_match(initString, match, power_supply_ocem_init_match, boost::match_extra)){
                std::string dev=match[1];
                slaveid=match[2];
                std::string maxcurr=match[3];
                std::string maxvoltage=match[3];
                PSLAPP<<"Allocating OcemE642X device \""<<slaveid<<"\""<<" on dev:\""<<dev<<"\""<<std::endl;
                power = new ::common::powersupply::OcemE642X(dev.c_str(),atoi(slaveid.c_str()),(float)atof(maxcurr.c_str()),(float)atof(maxvoltage.c_str()));
                if(power==NULL){
                      throw chaos::CException(1, "Cannot allocate resources for OcemE642X", "OcemDD::driverInit");
                }
            
            } else {
                 throw chaos::CException(1, "Bad parameters for OcemE642X <serial port>,<slaveid>,<maxcurr:maxvoltage>", "OcemDD::driverInit");

            }
        } else {
            throw chaos::CException(1, "Unsupported driver", "OcemDD::driverInit");

        }
    } else {
            throw chaos::CException(1, "Malformed initialisation string", "OcemDD::driverInit");
    
    }
    
    std::string ver;
    power->getSWVersion(ver,0);
    PSLAPP<<"DRIVER INSTANTITED \""<<ver<<"\""<<std::endl;
/*
    if(power->init()!=0){
        throw chaos::CException(1, "Initialisation of power supply \""+inputStr+"\" slaveid "+slaveid+" failed", "OcemDD::driverInit");
    }
  */
    
}

