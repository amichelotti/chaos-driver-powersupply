/*
 *	PowerSupplyControlUnit.cpp
 *	!CHOAS
 *	Created by Andrea Michelotti.
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

#include "PowerSupplyControlUnit.h"

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;

using namespace chaos::cu::driver_manager::driver;
using namespace driver::powersupply;
using namespace chaos;
#define CUAPP LAPP_ << "[PowerSupplyCU] - "


/*
 Construct a new CU with an identifier
 */
PowerSupplyControlUnit::PowerSupplyControlUnit(string _device_id,string _params):device_id(_device_id),params(_params) {
    powersupply_drv = NULL;
    current_sp = 0;
}

PowerSupplyControlUnit::~PowerSupplyControlUnit() {
	if(powersupply_drv != NULL) delete(powersupply_drv);
}

/*
 Return the default configuration
 */
void PowerSupplyControlUnit::unitDefineActionAndDataset() throw(chaos::CException) {
    //add managed device di
    setDeviceID(device_id);
 	
    addAttributeToDataSet("Current",
                          "current",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("CurrentSP",
                          "current Set Point",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("Voltage",
                          "voltage",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("Polarity",
                          "polarity",
                          DataType::TYPE_INT32,
                          DataType::Output);

    addAttributeToDataSet("Alarms",
                          "Alarms",
                          DataType::TYPE_INT64,
                          DataType::Output);

    addAttributeToDataSet("Status",
                          "status",
                          DataType::TYPE_STRING,
                          DataType::Output,256);
    
    
    addInputDoubleAttributeToDataSet<PowerSupplyControlUnit>("Setcurrent",
                                                                 "Set The current",
                                                                 this,
                                                                 &PowerSupplyControlUnit::setCurrent);
	
	addInputInt32AttributeToDataSet<PowerSupplyControlUnit>("Setpolarity",
                                                            "Set the polarity  1/-1/0",
                                                            this,
                                                            &PowerSupplyControlUnit::setPolarity);
    
	addInputInt64AttributeToDataSet<PowerSupplyControlUnit>("ClearAlarms",
                                                            "Clears the pending alarms",
                                                            this,
                                                            &PowerSupplyControlUnit::clearAlarms);
    
    addInputInt32AttributeToDataSet<PowerSupplyControlUnit>("SetStatus",
                                                                 "On[1]/StandBy[2]/Off[3]",
                                                                 this,
                                                                 &PowerSupplyControlUnit::setAlimStatus);
	
}

void PowerSupplyControlUnit::setCurrent(const std::string& deviceID, const double& current) {
    CUAPP<<"Command: setting current to:"<<current<<endl;
    if(powersupply_drv->setCurrentSP(current)==0){
        current_sp = current;
        if(powersupply_drv->startCurrentRamp()==0){
            CUAPP<<"Ramp Started"<<endl;
        }
    } else {
        CUAPP<<"Error setting current"<<endl;
    }
}
void PowerSupplyControlUnit::setAlimStatus(const std::string& deviceID, const int32_t& stat) {
    if(stat==1){
        CUAPP<<"Command: state to ON"<<endl;
        if(powersupply_drv->poweron()==0){
            CUAPP<<"OK"<<endl;
        }
    } else if(stat==2){
        CUAPP<<"Command: state to STANDBY"<<endl;
        if(powersupply_drv->standby()==0){
            CUAPP<<"OK"<<endl;
        }
    } else if(stat==3){
        CUAPP<<"Command: state to OFF !!! PAY ATTENTION "<<endl;
        if(powersupply_drv->shutdown()==0){
            CUAPP<<"OK"<<endl;
        }
    }
}
void PowerSupplyControlUnit::setPolarity(const std::string& deviceID, const int32_t& pol) {
    CUAPP<<"Command: set polarity to :"<<pol<<endl;

    if(powersupply_drv->setPolarity(pol)==0){
        CUAPP<<"OK"<<endl;
    }
}
void PowerSupplyControlUnit::clearAlarms(const std::string& deviceID, const int64_t& alrm) {
    CUAPP<<"Command: Clear Alarms"<<endl;
    
    if(powersupply_drv->resetAlarms(alrm)==0){
        CUAPP<<"OK"<<endl;
    }
}


void PowerSupplyControlUnit::unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver) {
	chaos::cu::driver_manager::driver::DrvRequestInfo drv1 = {"GenericPowerSupplyDD", "1.0.0", params.c_str() };
	neededDriver.push_back(drv1);
}

// Abstract method for the initialization of the control unit
void PowerSupplyControlUnit::unitInit() throw(chaos::CException) {
	CUAPP "unitInit";
    chaos::cu::cu_driver::DriverAccessor * power_supply_accessor=AbstractControlUnit::getAccessoInstanceByIndex(0);
    if(power_supply_accessor==NULL){
        throw chaos::CException(1, "Cannot retrieve the requested driver", __FUNCTION__);
    }
    powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
    if(powersupply_drv==NULL){
            throw chaos::CException(1, "Cannot allocate driver resources", __FUNCTION__);
    }
    
    if(powersupply_drv->getHWVersion(device_hw,1000)!=0){
            throw  chaos::CException(1, "Error cannot get HW version", __FUNCTION__);
    }
}

// Abstract method for the start of the control unit
void PowerSupplyControlUnit::unitStart() throw(chaos::CException) {
	CUAPP "unitStart";
}


//intervalled scheduled method
void PowerSupplyControlUnit::unitRun() throw(chaos::CException) {
    static int messageID=0;
    float current,voltage;
    int change=0,polarity,stato;
    uint64_t alarms;
    string desc;
    CDataWrapper *acquiredData = getNewDataWrapper();
    if(powersupply_drv->getVoltageOutput(&voltage)==0){
        acquiredData->addDoubleValue("voltage", voltage);
        change ++;
    }
    acquiredData->addDoubleValue("currentSP", current_sp);

    if(powersupply_drv->getCurrentOutput(&current)==0){
        acquiredData->addDoubleValue("current", current);
        change++;
    }
    
    if(powersupply_drv->getPolarity(&polarity)==0){
        acquiredData->addInt32Value("Polarity", polarity);
        change++;
    }
    if(powersupply_drv->getAlarms(&alarms)==0){
        acquiredData->addInt64Value("Alarms", alarms);
        change++;
    }
   
    if(powersupply_drv->getState(&stato,desc)==0){
        acquiredData->addStringValue("Status", desc);
        change++;
    }
    
    if(change){
        acquiredData->addInt32Value("id", ++messageID);
        pushDataSet(acquiredData);

    }
}

// Abstract method for the stop of the control unit
void PowerSupplyControlUnit::unitStop() throw(chaos::CException) {
	CUAPP "unitStop";
}

// Abstract method for the deinit of the control unit
void PowerSupplyControlUnit::unitDeinit() throw(chaos::CException) {
	CUAPP "unitDeinit";
}
