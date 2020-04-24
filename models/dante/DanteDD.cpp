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
chaos::driver::powersupply::DanteDD::DanteDD():current(0) {
	power = NULL;

}

//default descrutcor
chaos::driver::powersupply::DanteDD::~DanteDD() {

}
void chaos::driver::powersupply::DanteDD::driverInit(const chaos::common::data::CDataWrapper& json) throw(chaos::CException){
	dante.driverInit((const char*)json.getCompliantJSONString().c_str());
}

void chaos::driver::powersupply::DanteDD::driverInit(const char *initParameter) throw(chaos::CException) {
	//check the input parameter
	boost::smatch match;
	std::string inputStr = initParameter;
	std::string slaveid;
	PSLAPP << "Init  driver initialisation string:\""<<initParameter<<"\""<<std::endl;
	
	dante.driverInit(initParameter);
	power=(::common::powersupply::AbstractPowerSupply*)this;
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
		dante.driverDeinit();
}


int chaos::driver::powersupply::DanteDD::setPolarity(int pol,uint32_t timeo_ms){
	DANTE_DBG<<"NOT IMPLEMENTED ";

	return 0;
}
            
        
int chaos::driver::powersupply::DanteDD::getPolarity(int* pol,uint32_t timeo_ms){
	DANTE_DBG<<"NOT IMPLEMENTED ";

	return 0;
}
                    
int chaos::driver::powersupply::DanteDD::setCurrentSP(float _current,uint32_t timeo_ms){
	current=_current;
	return 0;
}
int chaos::driver::powersupply::DanteDD::getCurrentSP(float* _current,uint32_t timeo_ms){
	*_current=current;
	return 0;
}
            
int chaos::driver::powersupply::DanteDD::startCurrentRamp(uint32_t timeo_ms){
	chaos::common::data::CDataWrapper cd;
	cd.addDoubleValue("value",current);
	chaos::common::data::CDWUniquePtr ret=dante.postData("setCurrent",&cd);
	return 0;
}
            
            
int chaos::driver::powersupply::DanteDD::getVoltageOutput(float* volt,uint32_t timeo_ms){
	DANTE_DBG<<"NOT IMPLEMENTED ";
	return 0;

}
            
            
int chaos::driver::powersupply::DanteDD::getCurrentOutput(float* _curr,uint32_t timeo_m){
	double curr=0;
	int ret=dante.getData("current",(void*)&curr);
	*_curr=curr;
	return ret;
}
            
            
         
int chaos::driver::powersupply::DanteDD::setCurrentRampSpeed(float asup,float asdown,uint32_t timeo_ms){
	chaos::common::data::CDataWrapper cd;
	cd.addDoubleValue("value",asup);
	chaos::common::data::CDWUniquePtr ret=dante.postData("setSlop",&cd);
	if(ret.get()&&ret->hasKey(::driver::data_import::PROT_ERROR)){
		chaos::common::data::CDWUniquePtr rr=ret->getCSDataValue(::driver::data_import::PROT_ERROR);
		DANTE_ERR<<" DANTE_ERRor occurred:"<<rr->getStringValue("msg");
		return -1;
	}
	return 0;
}
            
            
          
int chaos::driver::powersupply::DanteDD::resetAlarms(uint64_t alrm,uint32_t timeo_ms){
	chaos::common::data::CDWUniquePtr ret=dante.postData("resetDANTE_ERRors",NULL);
if(ret.get()&&ret->hasKey(::driver::data_import::PROT_ERROR)){
		chaos::common::data::CDWUniquePtr rr=ret->getCSDataValue(::driver::data_import::PROT_ERROR);
		DANTE_ERR<<" DANTE_ERRor occurred:"<<rr->getStringValue("msg");
		return -1;
	}
	return 0;
}
            
int chaos::driver::powersupply::DanteDD::getAlarms(uint64_t*alrm,uint32_t timeo_ms){
 *alrm=0;
 return 0;
}
            
            
int chaos::driver::powersupply::DanteDD::shutdown(uint32_t timeo_ms){
	chaos::common::data::CDWUniquePtr ret=dante.postData("powerOff",NULL);
if(ret.get()&&ret->hasKey(::driver::data_import::PROT_ERROR)){
		chaos::common::data::CDWUniquePtr rr=ret->getCSDataValue(::driver::data_import::PROT_ERROR);
		DANTE_ERR<<" DANTE_ERRor occurred:"<<rr->getStringValue("msg");
		return -1;
	}
	return 0;

}
            
            
int chaos::driver::powersupply::DanteDD::standby(uint32_t timeo_ms){
	chaos::common::data::CDWUniquePtr ret=dante.postData("standby",NULL);
	if(ret.get()&&ret->hasKey(::driver::data_import::PROT_ERROR)){
		chaos::common::data::CDWUniquePtr rr=ret->getCSDataValue(::driver::data_import::PROT_ERROR);
		DANTE_ERR<<" DANTE_ERRor occurred:"<<rr->getStringValue("msg");
		return -1;
	}
		return 0;

}
int chaos::driver::powersupply::DanteDD::poweron(uint32_t timeo_ms){
	chaos::common::data::CDWUniquePtr ret=dante.postData("operational",NULL);
	if(ret.get()&&ret->hasKey(::driver::data_import::PROT_ERROR)){
		chaos::common::data::CDWUniquePtr rr=ret->getCSDataValue(::driver::data_import::PROT_ERROR);
		DANTE_ERR<<" DANTE_ERRor occurred:"<<rr->getStringValue("msg");
		return -1;
	}
		return 0;

}
            
            
 int chaos::driver::powersupply::DanteDD::getState(int* state,std::string& desc,uint32_t timeo_ms){
	 return 0;
 }
            
int chaos::driver::powersupply::DanteDD::init(){
	return 0;
}
            
void chaos::driver::powersupply::DanteDD::deinit(){
//	return 0;
}
            
int chaos::driver::powersupply::DanteDD::getSWVersion(std::string& version,uint32_t timeo_ms){
	version = "1.0.0";
	return 0;
}
            
int chaos::driver::powersupply::DanteDD::getHWVersion(std::string& version,uint32_t timeo_ms){
	version = "1.0.0";
	return 0;
}
            
int chaos::driver::powersupply::DanteDD::getCurrentSensibility(float *sens){
	return 0;
}
int chaos::driver::powersupply::DanteDD::getVoltageSensibility(float *sens){
	return 0;
}
            
            
int chaos::driver::powersupply::DanteDD::setCurrentSensibility(float sens){
	return 0;
}
            
        
int chaos::driver::powersupply::DanteDD::setVoltageSensibility(float sens){
	return 0;
}
int chaos::driver::powersupply::DanteDD::getMaxMinCurrent(float*max,float*min){
	return 0;
}
            
int chaos::driver::powersupply::DanteDD::getMaxMinVoltage(float*max,float*min){
	return 0;
}
int chaos::driver::powersupply::DanteDD::getAlarmDesc(uint64_t* alarm){

}
            
int chaos::driver::powersupply::DanteDD::forceMaxCurrent(float max){
	return 0;
}
            
            
int chaos::driver::powersupply::DanteDD::forceMaxVoltage(float max){
	return 0;
}
uint64_t chaos::driver::powersupply::DanteDD::getFeatures(){
	return 0;
}
