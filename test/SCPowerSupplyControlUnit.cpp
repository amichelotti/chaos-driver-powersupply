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
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

//---comands----
#include "CmdPSDefault.h"
#include "CmdPSMode.h"
#include "CmdPSReset.h"
#include "CmdPSSetSlope.h"
#include "CmdPSSetCurrent.h"
#include "CmdSetPolarity.h"

using namespace chaos;

using namespace chaos::common::data;
using namespace chaos::common::batch_command;

using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;

namespace own =  ::driver::powersupply;

#define SCCUAPP LAPP_ << "[SCPowerSupplyControlUnit - " << getCUID() << "] - "

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(own::SCPowerSupplyControlUnit)

/*
 Construct a new CU with an identifier
 */
own::SCPowerSupplyControlUnit::SCPowerSupplyControlUnit(const string& _control_unit_id,
														const string& _control_unit_param,
														const ControlUnitDriverList& _control_unit_drivers):
//call base constructor
chaos::cu::control_manager::SCAbstractControlUnit(_control_unit_id,
												  _control_unit_param,
												  _control_unit_drivers){
	
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
    //install all command
    installCommand<CmdPSDefault>(CMD_PS_DEFAULT_ALIAS);
    installCommand<CmdPSMode>(CMD_PS_MODE_ALIAS);
	installCommand<CmdPSReset>(CMD_PS_RESET_ALIAS);
	installCommand<CmdPSSetSlope>(CMD_PS_SET_SLOPE_ALIAS);
	installCommand<CmdPSSetCurrent>(CMD_PS_SET_CURRENT_ALIAS);
	installCommand<CmdSetPolarity>(CMD_PS_SET_POLARITY_ALIAS);
    //set it has default
	setDefaultCommand(CMD_PS_DEFAULT_ALIAS);
    
    //setup the dataset
	addAttributeToDataSet("current",
                          "current",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("current_sp",
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
	
	addAttributeToDataSet("status_id",
                          "status_id",
                          DataType::TYPE_INT32,
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
	/*
     * JAVASCRIPT INTERFACE
     */
    addAttributeToDataSet("on",
                          "power supply is on",
                          DataType::TYPE_INT32,
                          DataType::Output);
	addAttributeToDataSet("stby",
                          "power supply is on standby",
                          DataType::TYPE_INT32,
                          DataType::Output);
    addAttributeToDataSet("alarm",
                          "power supply alarm",
                          DataType::TYPE_INT32,
                          DataType::Output);
    
    
    ///
	addAttributeToDataSet("slope_up",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
	
	addAttributeToDataSet("slope_down",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
	
	addAttributeToDataSet("driver_timeout",
                          "Driver timeout in milliseconds",
                          DataType::TYPE_INT32,
                          DataType::Input);
	
	addAttributeToDataSet("command_timeout",
                          "General command timeout in microseconds",
                          DataType::TYPE_INT32,
                          DataType::Input);
	
	addAttributeToDataSet("delta_setpoint",
                          "Delta of the setpoint",
                          DataType::TYPE_INT32,
                          DataType::Input);
	
	addAttributeToDataSet("setpoint_affinity",
                          "Delta of the setpoint",
                          DataType::TYPE_INT32,
                          DataType::Input);
	//define the custom share, across slow command, variable
}

void own::SCPowerSupplyControlUnit::defineSharedVariable() {
	
}

// Abstract method for the initialization of the control unit
void own::SCPowerSupplyControlUnit::unitInit() throw(CException) {
	SCCUAPP "unitInit";
    int err = 0;
	int state_id;
    std::string max_range;
    std::string min_range;
    std::string state_str;
    RangeValueInfo current_sp_attr_info;
    
	double *asup = getVariableValue(IOCAttributeSharedCache::SVD_INPUT, "slope_up")->getCurrentValue<double>();
	double *asdown = getVariableValue(IOCAttributeSharedCache::SVD_INPUT, "slope_down")->getCurrentValue<double>();
    int32_t *status_id = getVariableValue(IOCAttributeSharedCache::SVD_INPUT, "status_id")->getCurrentValue<int32_t>();

    
    
	chaos::cu::driver_manager::driver::DriverAccessor * power_supply_accessor=AbstractControlUnit::getAccessoInstanceByIndex(0);
	if(power_supply_accessor==NULL){
        throw chaos::CException(1, "Cannot retrieve the requested driver", __FUNCTION__);
    }
	powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
	if(powersupply_drv==NULL){
		throw chaos::CException(1, "Cannot allocate driver resources", __FUNCTION__);
	}
    
        //check mandatory default values
    /*
     */
    SCCUAPP << "check mandatory default values";
	getAttributeRangeValueInfo("current_sp", current_sp_attr_info);
        
        // REQUIRE MIN MAX SET IN THE MDS
    if(!current_sp_attr_info.maxRange.size() || !current_sp_attr_info.minRange.size()) {
       throw chaos::CException(1, "current set point need to have max and min", __FUNCTION__);
    }
    
    SCCUAPP << "current_sp max="<< (max_range = current_sp_attr_info.maxRange);
    SCCUAPP << "current_sp min="<< (min_range = current_sp_attr_info.minRange);
    
	// retrive the attribute description from the device database
    /*
    * current_sp_attr_info.reset();
    */
	getAttributeRangeValueInfo("slope_up", current_sp_attr_info);
	if(current_sp_attr_info.defaultValue.size()) {
        *asup = boost::lexical_cast<float>(current_sp_attr_info.defaultValue);
        SCCUAPP << "slope_up = "<<*asup;
	} else {
        SCCUAPP << "slope_up not set we need to compute it";
        *asup = boost::lexical_cast<float>(max_range)/20.0;
        SCCUAPP << "slope_up computed = " << *asup;
    }
	
	current_sp_attr_info.reset();
	getAttributeRangeValueInfo("slope_down", current_sp_attr_info);
	if(current_sp_attr_info.defaultValue.size()) {
		*asdown = boost::lexical_cast<float>(current_sp_attr_info.defaultValue);
        SCCUAPP << "slope_down = "<<*asup;
	} else {
        SCCUAPP << "slope_down not set we need to compute it";
        *asdown = boost::lexical_cast<float>(max_range)/20.0;
        SCCUAPP << "slope_down computed = " << *asdown;
    }
    
	if(powersupply_drv->getState(&state_id, state_str, 30000)!=0) {
		throw  chaos::CException(1, "Error getting the state of the powersupply, possibily off", __FUNCTION__);
    }
	*status_id = state_id;
    
    if(powersupply_drv->getHWVersion(device_hw,1000)==0){
		SCCUAPP << "hardware found: \"" << device_hw<<"\"";
    }

	if( (*asup > 0) && (*asdown > 0)) {
		SCCUAPP << "set default slope value up:"<<*asup<<" down:"<<*asdown;
		if((err = powersupply_drv->setCurrentRampSpeed(*asup, *asdown) )!= 0) {
                     throw chaos::CException(1, "Error setting slope ", __FUNCTION__); 
                     //TODO: check the  boost::bad_format_string: format-string is ill-formed
			//throw chaos::CException(2, boost::str( boost::format("Error %1 setting the slope in state %2%[%3%]") % err % state_str % state_id), std::string(__FUNCTION__));
		}
	} else {
		throw chaos::CException(2, "No default slope value found", std::string(__FUNCTION__));
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