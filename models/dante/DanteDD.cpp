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
#include "DanteDD.h"

#include <string>
#include <boost/regex.hpp>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <chaos/common/data/CDataWrapper.h>
#include <common/powersupply/core/AbstractPowerSupply.h>
#include "PSDanteDriver.h"

#define DANTE_INFO INFO_LOG(DanteDD)
#define DANTE_DBG DBG_LOG(DanteDD)
#define DANTE_ERR ERR_LOG(DanteDD)

//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(DanteDD, 1.0.0, chaos::driver::powersupply::DanteDD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::powersupply::DanteDD, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::powersupply::DanteDD)
CLOSE_REGISTER_PLUGIN

using namespace driver::data_import;

//default constructor definition
chaos::driver::powersupply::DanteDD::DanteDD() {
	power = NULL;

}

//default descrutcor
chaos::driver::powersupply::DanteDD::~DanteDD() {

}
void chaos::driver::powersupply::DanteDD::driverInit(const chaos::common::data::CDataWrapper& json) throw(chaos::CException){
	driverInit((const char*)json.getCompliantJSONString().c_str());
}

void chaos::driver::powersupply::DanteDD::driverInit(const char *initParameter) throw(chaos::CException) {
	//check the input parameter
	boost::smatch match;
	std::string inputStr = initParameter;
	std::string slaveid;
	PSLAPP << "Init  driver initialisation string:\""<<initParameter<<"\""<<std::endl;
	
	power=(::common::powersupply::AbstractPowerSupply*)new PSDanteDriver(initParameter);
	std::string ver;
	power->getSWVersion(ver,0);
	PSLAPP<<"DRIVER INSTANTITED \""<<ver<<"\""<<std::endl;
	/*
    if(power->init()!=0){
        throw chaos::CException(1, "Initialisation of power supply \""+inputStr+"\" slaveid "+slaveid+" failed", "DanteDD::driverInit");
    }
	 */

}

void chaos::driver::powersupply::DanteDD::driverDeinit() throw(chaos::CException) {
	if(power){
		delete power;
		power=NULL;
	}
}

