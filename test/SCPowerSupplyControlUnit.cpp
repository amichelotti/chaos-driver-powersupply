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

using namespace driver::powersupply;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace driver::powersupply;
using namespace chaos;
/*
 Construct a new CU with an identifier
 */
SCPowerSupplyControlUnit::SCPowerSupplyControlUnit(string _device_id, string _params):device_id(_device_id), params(_params),  powersupply_drv(NULL) {
	
}

/*
 Base destructor
 */
SCPowerSupplyControlUnit::~SCPowerSupplyControlUnit() {
	
}


/*
 Return the default configuration
 */
void SCPowerSupplyControlUnit::unitDefineActionAndDataset() throw(chaos::CException) {
    //set the base information
    //RangeValueInfo rangeInfoTemp;
    
    //add managed device di
    setDeviceID(device_id);
    
    //install a command
    //installCommand<SinWaveCommand>("sinwave_base");
    
    //set it has default
	// setDefaultCommand("sinwave_base");
    
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
	
	addAttributeToDataSet("ps_state",
                          "Current powersupply state",
                          DataType::TYPE_STRING,
                          DataType::Output,22);
	
	addAttributeToDataSet("slope_up",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
	
	addAttributeToDataSet("slope_down",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
	
	addAttributeToDataSet("timeout",
                          "command timeout in milliseconds",
                          DataType::TYPE_INT32,
                          DataType::Input);
	//define the custom share, across slow command, variable
   
	//here are defined the custom shared variable
    addSharedVariable("pw_sm", sizeof(void*), chaos::DataType::TYPE_BYTEARRAY);
    setSharedVariableValue("pw_sm", (void*)&powersupply_sm, sizeof(void*));
}

void SCPowerSupplyControlUnit::defineSharedVariable() {

}

void SCPowerSupplyControlUnit::unitDefineDriver(std::vector<DrvRequestInfo>& neededDriver) {
	DrvRequestInfo drv1 = {"GenericPowerSupplyDD", "1.0.0", params.c_str() };
	neededDriver.push_back(drv1);
}

// Abstract method for the initialization of the control unit
void SCPowerSupplyControlUnit::unitInit() throw(CException) {
	
}

// Abstract method for the start of the control unit
void SCPowerSupplyControlUnit::unitStart() throw(CException) {
	
}

// Abstract method for the stop of the control unit
void SCPowerSupplyControlUnit::unitStop() throw(CException) {
	
}

// Abstract method for the deinit of the control unit
void SCPowerSupplyControlUnit::unitDeinit() throw(CException) {
	
}