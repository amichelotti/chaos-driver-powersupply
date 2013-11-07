/*
 *	SCPowerSupplyControlUnit
 *	!CHOAS
 *	Created by Claudio Bisegni.
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

#include "SCPowerSupplyControlUnit.h"

//---comands----
#include "CmdPSDefault.h"
#include "CmdPSMode.h"

using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos;
namespace own =  ::driver::powersupply;

#define SCCUAPP LAPP_ << "[SCPowerSupplyControlUnit - " << device_id << "] - "

/*
 Construct a new CU with an identifier
 */
own::SCPowerSupplyControlUnit::SCPowerSupplyControlUnit(string _device_id, string _params):device_id(_device_id), params(_params),  powersupply_drv(NULL) {
	
}

/*
 Base destructor
 */
own::SCPowerSupplyControlUnit::~SCPowerSupplyControlUnit() {
	
}


/*
 Return the default configuration
 */
void own::SCPowerSupplyControlUnit::unitDefineActionAndDataset() throw(chaos::CException) {
    //set the base information
    //RangeValueInfo rangeInfoTemp;
    
    //add managed device di
    setDeviceID(device_id);
    
    //install all command
    installCommand<CmdPSDefault>(CMD_PS_DEFAULT_ALIAS);
    installCommand<CmdPSMode>(CMD_PS_MODE_ALIAS);
	
    //set it has default
	setDefaultCommand(CMD_PS_DEFAULT_ALIAS);
    
    //setup the dataset
	addAttributeToDataSet("current",
                          "current",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("currentSP",
                          "current Set Point",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("voltage",
                          "voltage",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("polarity",
                          "polarity",
                          DataType::TYPE_INT32,
                          DataType::Output);
	
    addAttributeToDataSet("alarms",
                          "Alarms",
                          DataType::TYPE_INT64,
                          DataType::Output);
	
    addAttributeToDataSet("status",
                          "status",
                          DataType::TYPE_STRING,
                          DataType::Output,256);
	
	addAttributeToDataSet("dev_state",
                          "Bit field device state",
                          DataType::TYPE_INT64,
                          DataType::Output);
	
	addAttributeToDataSet("cmd_last_error",
                          "Last Erroro occurred",
                          DataType::TYPE_STRING,
                          DataType::Output,256);
	
	addAttributeToDataSet("slope_up",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
	
	addAttributeToDataSet("slope_down",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
	
	addAttributeToDataSet("driver_timeout",
                          "driver timeout in milliseconds",
                          DataType::TYPE_INT32,
                          DataType::Input);
	
	addAttributeToDataSet("command_timeout",
                          "command timeout in microseconds",
                          DataType::TYPE_INT32,
                          DataType::Input);
	//define the custom share, across slow command, variable
}

void own::SCPowerSupplyControlUnit::defineSharedVariable() {
	
}

void own::SCPowerSupplyControlUnit::unitDefineDriver(std::vector<DrvRequestInfo>& neededDriver) {
	DrvRequestInfo drv1 = {"GenericPowerSupplyDD", "1.0.0", params.c_str() };
	neededDriver.push_back(drv1);
}

// Abstract method for the initialization of the control unit
void own::SCPowerSupplyControlUnit::unitInit() throw(CException) {
	SCCUAPP "unitInit";
	int state_id;
	std::string state_str;
	chaos::cu::cu_driver::DriverAccessor * power_supply_accessor=AbstractControlUnit::getAccessoInstanceByIndex(0);
	if(power_supply_accessor==NULL){
        throw chaos::CException(1, "Cannot retrieve the requested driver", __FUNCTION__);
    }
	powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
	if(powersupply_drv==NULL){
		throw chaos::CException(1, "Cannot allocate driver resources", __FUNCTION__);
	}
    
	if(powersupply_drv->getState(&state_id, state_str, 30000)!=0){
		throw  chaos::CException(1, "Error getting the state of the powersupply, possibily off", __FUNCTION__);
    }
	
    if(powersupply_drv->getHWVersion(device_hw,1000)==0){
		SCCUAPP << "hardware found " << "device_hw";
    }
}

// Abstract method for the start of the control unit
void own::SCPowerSupplyControlUnit::unitStart() throw(CException) {
	
}

// Abstract method for the stop of the control unit
void own::SCPowerSupplyControlUnit::unitStop() throw(CException) {
	
}

// Abstract method for the deinit of the control unit
void own::SCPowerSupplyControlUnit::unitDeinit() throw(CException) {
	
}