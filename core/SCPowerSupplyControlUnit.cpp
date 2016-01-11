/*
 *	SCPowerSupplyControlUnit
 *	!CHAOS
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



#define SCCUAPP INFO_LOG(SCPowerSupplyControlUnit)
#define SCCUDBG DBG_LOG(SCPowerSupplyControlUnit)
#define SCCUERR ERR_LOG(SCPowerSupplyControlUnit)

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::powersupply::SCPowerSupplyControlUnit)

/*
 Construct a new CU with an identifier
 */
::driver::powersupply::SCPowerSupplyControlUnit::SCPowerSupplyControlUnit(const string& _control_unit_id,
														const string& _control_unit_param,
														const ControlUnitDriverList& _control_unit_drivers):
//call base constructor
chaos::cu::control_manager::SCAbstractControlUnit(_control_unit_id,
												  _control_unit_param,
												  _control_unit_drivers){
    powersupply_drv = NULL;
}

/*
 Base destructor
 */
::driver::powersupply::SCPowerSupplyControlUnit::~SCPowerSupplyControlUnit() {
	if(powersupply_drv){
		delete(powersupply_drv);
	}
}


/*
 Return the default configuration
 */
void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineActionAndDataset() throw(chaos::CException) {
	//install all command
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSDefault),true);
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSMode));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSReset));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSSetSlope));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSSetCurrent));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdSetPolarity));
	
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
        addAttributeToDataSet("max_current",
						  "The maximum current applicable",
						  DataType::TYPE_DOUBLE,
						  DataType::Input);
	
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

void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineCustomAttribute() {
	
}

// Abstract method for the initialization of the control unit
void ::driver::powersupply::SCPowerSupplyControlUnit::unitInit() throw(CException) {
	SCCUAPP "unitInit";
	int err = 0;
	int state_id;
	std::string max_range;
	std::string min_range;
	std::string state_str;
	RangeValueInfo attr_info;
	
	const double *asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "slope_up");
	const double *asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "slope_down");
	int32_t *status_id = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "status_id");
	
	
	
	chaos::cu::driver_manager::driver::DriverAccessor * power_supply_accessor = getAccessoInstanceByIndex(0);
	if(power_supply_accessor==NULL){
		throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
	if(powersupply_drv==NULL){
		throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}
	
	//check mandatory default values
	/*
	 */
	SCCUAPP << "check mandatory default values";
	getAttributeRangeValueInfo("max_current", attr_info);
	
	// REQUIRE MIN MAX SET IN THE MDS
	if(attr_info.maxRange.size() ) {
            SCCUAPP << "max_current max="<< (max_range = attr_info.maxRange);
	}
	
	
	// retrive the attribute description from the device database
	/*
	 * current_sp_attr_info.reset();
	 */
	if(*asup <= 0) {
		throw chaos::CException(-4, "No slop up speed set", __FUNCTION__);
	}
	if(*asdown <= 0) {
		throw chaos::CException(-5, "No slop down speed set", __FUNCTION__);
	}
	
	
	if(powersupply_drv->getState(&state_id, state_str, 30000)!=0) {
		throw  chaos::CException(-6, "Error getting the state of the powersupply, possibily off", __FUNCTION__);
	}
	*status_id = state_id;
	//notify change on status_id cached attribute
	getAttributeCache()->setOutputDomainAsChanged();
	
	if(powersupply_drv->getHWVersion(device_hw,1000)==0){
		SCCUAPP << "hardware found: \"" << device_hw<<"\"";
	}
	
	
	SCCUAPP << "set default slope value up:"<<*asup<<" down:"<<*asdown;
        err = powersupply_drv->setCurrentRampSpeed(*asup, *asdown);
	if( (err!= chaos::ErrorCode::EC_NO_ERROR)  && (err!=chaos::ErrorCode::EC_NODE_OPERATION_NOT_SUPPORTED) ) {
		throw chaos::CException(-7, "Error setting slope ", __FUNCTION__);
		//TODO: check the  boost::bad_format_string: format-string is ill-formed
		//throw chaos::CException(2, boost::str( boost::format("Error %1 setting the slope in state %2%[%3%]") % err % state_str % state_id), std::string(__FUNCTION__));
	}
}

// Abstract method for the start of the control unit
void ::driver::powersupply::SCPowerSupplyControlUnit::unitStart() throw(CException) {
	
}

// Abstract method for the stop of the control unit
void ::driver::powersupply::SCPowerSupplyControlUnit::unitStop() throw(CException) {
	
}

// Abstract method for the deinit of the control unit
void ::driver::powersupply::SCPowerSupplyControlUnit::unitDeinit() throw(CException) {
	
}

//! restore the control unit to snapshot
bool ::driver::powersupply::SCPowerSupplyControlUnit::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache * const snapshot_cache) throw(chaos::CException) {
	SCCUAPP << "Check if restore cache has the needed data";
	//check if in the restore cache we have all information we need
	if(!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("status_id")) return false;
	if(!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("polarity")) return false;
	if(!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("current_sp")) return false;

	SCCUAPP << "Start the restore of the powersupply";

	bool cmd_result = true;
	//get actual state
	double *now_current_sp = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "current_sp");
	int32_t *now_status_id = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "status_id");
	int32_t *now_polarity = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");

	//chec the restore polarity
	int32_t restore_polarity = 0;
	if(snapshot_cache->getSharedDomain(DOMAIN_INPUT).hasAttribute(CMD_PS_SET_POLARITY_ALIAS)) {
		//we use last command sent for polarity
		auto_ptr<CDataWrapper> polarity_cmd_pack(snapshot_cache->getAttributeValue(DOMAIN_INPUT, CMD_PS_SET_POLARITY_ALIAS)->getValueAsCDatawrapperPtr(true));
		restore_polarity = polarity_cmd_pack->getInt32Value(CMD_PS_SET_POLARITY_VALUE);
	} else {
		restore_polarity = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "polarity")->getValuePtr<int32_t>();
	}


	if(*now_polarity != restore_polarity) {
		//we need to change the polarity
		SCCUAPP << "Change the polarity from:" << *now_polarity << " to:" << restore_polarity;

		//put in standby
		SCCUAPP << "Put powersupply in standby";
		if(setCurrent(0.0)) {
			if(powerStandby()) {
				SCCUAPP << "Powersupply is gone in standby";
			} else {
				LOG_AND_TROW(SCCUERR, 2, "Power supply is not gone in standby");
			}
		} else {
			LOG_AND_TROW(SCCUERR, 1, "Power supply is not gone to 0 ampere");
		}

		//set the polarity
		SCCUAPP << "Apply new polarity";
		if(!setPolarity(restore_polarity)) {
			LOG_AND_TROW_FORMATTED(SCCUERR, 3, "Power supply is not gone to restore polarity %1%", %restore_polarity);
		}
	}

	int32_t restore_status_id = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "status_id")->getValuePtr<int32_t>();
	if(*now_status_id != restore_status_id) {
		SCCUAPP << "Change the status from:" << *now_status_id << " to:" << restore_status_id;
		//we need to change the sate
		switch ((*now_status_id = restore_status_id)) {
			case 0x2:
				SCCUAPP << "Put powersupply in on state to restore his status";
				if(!powerON()) {
					LOG_AND_TROW(SCCUERR, 4, "Power supply is not gone to restore 'power on' state");
				}
				break;
			case 0x8:
				//set the powersupply on stand-by
				SCCUAPP << "Put powersupply in standby state to restore his status";
				if(setCurrent(0.0)) {
					if(powerStandby()) {
						SCCUAPP << "Powersupply is gone in standby";
					} else {
						LOG_AND_TROW(SCCUERR, 5, "Power supply is not gone in standby");
					}
				} else {
					LOG_AND_TROW(SCCUERR, 6, "Power supply is not gone to 0 ampere");
				}
				break;

			default:
				return false;
				break;
		}
	}

	double restore_current_sp = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "current_sp")->getValuePtr<double>();
	if(!setCurrent(restore_current_sp)) {
		LOG_AND_TROW_FORMATTED(SCCUERR, 6, "Power supply is not gone to restore 'current setpoint %1%' state", %restore_current_sp);
	}
	return true;
}
//-----------utility methdo for the restore operation---------
bool ::driver::powersupply::SCPowerSupplyControlUnit::powerON(bool sync) {
	uint64_t cmd_id;
	bool result = true;
	std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addInt32Value(CMD_PS_MODE_TYPE, 1);
	//send command
	submitBatchCommand(CMD_PS_MODE_ALIAS,
					  cmd_pack.release(),
					   cmd_id,
					   0,
					   50,
					   SubmissionRuleType::SUBMIT_AND_Stack);
	if(sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}
bool ::driver::powersupply::SCPowerSupplyControlUnit::powerStandby(bool sync) {
	uint64_t cmd_id;
	bool result = true;
	std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addInt32Value(CMD_PS_MODE_TYPE, 0);
	//send command
	submitBatchCommand(CMD_PS_MODE_ALIAS,
					   cmd_pack.release(),
					   cmd_id,
					   0,
					   50,
					   SubmissionRuleType::SUBMIT_AND_Stack);
	if(sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setPolarity(int polarity,
																  bool sync) {
	uint64_t cmd_id;
	bool result = true;
	std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addInt32Value(CMD_PS_SET_POLARITY_VALUE, polarity);
	//send command
	submitBatchCommand(CMD_PS_SET_POLARITY_ALIAS,
					   cmd_pack.release(),
					   cmd_id,
					   0,
					   50,
					   SubmissionRuleType::SUBMIT_AND_Stack);
	if(sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setCurrent(double current_set_point,
																 bool sync) {
	uint64_t cmd_id;
	bool result = true;
	std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addDoubleValue(CMD_PS_SET_CURRENT, current_set_point);
	//send command
	submitBatchCommand(CMD_PS_SET_CURRENT_ALIAS,
					   cmd_pack.release(),
					   cmd_id,
					   0,
					   50,
					   SubmissionRuleType::SUBMIT_AND_Stack);
	if(sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setRampSpeed(double sup,
																   double sdown,
																   bool sync) {
	uint64_t cmd_id;
	bool result = true;
	std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addDoubleValue(CMD_PS_SET_SLOPE_UP, sup);
	cmd_pack->addDoubleValue(CMD_PS_SET_SLOPE_DOWN, sdown);
	//send command
	submitBatchCommand(CMD_PS_SET_SLOPE_ALIAS,
					   cmd_pack.release(),
					   cmd_id,
					   0,
					   50,
					   SubmissionRuleType::SUBMIT_AND_Stack);
	if (sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::whaitOnCommandID(uint64_t cmd_id) {
	std::auto_ptr<CommandState> cmd_state;
	do{
		cmd_state = getStateForCommandID(cmd_id);
		if(!cmd_state.get()) break;

		switch (cmd_state->last_event) {
			case BatchCommandEventType::EVT_QUEUED:
				SCCUAPP << cmd_id << " -> QUEUED";
				break;
			case BatchCommandEventType::EVT_RUNNING:
				SCCUAPP << cmd_id << " -> RUNNING";
				break;
			case BatchCommandEventType::EVT_WAITING:
				SCCUAPP << cmd_id << " -> WAITING";
				break;
			case BatchCommandEventType::EVT_PAUSED:
				SCCUAPP << cmd_id << " -> PAUSED";
				break;
			case BatchCommandEventType::EVT_KILLED:
				SCCUAPP << cmd_id << " -> KILLED";
				break;
			case BatchCommandEventType::EVT_COMPLETED:
				SCCUAPP << cmd_id << " -> COMPLETED";
				break;
			case BatchCommandEventType::EVT_FAULT:
				SCCUAPP << cmd_id << " -> FALUT";
				break;
		}
	}while(cmd_state->last_event != BatchCommandEventType::EVT_COMPLETED &&
			cmd_state->last_event != BatchCommandEventType::EVT_FAULT &&
			cmd_state->last_event != BatchCommandEventType::EVT_KILLED);
	return (cmd_state.get() &&
			cmd_state->last_event == BatchCommandEventType::EVT_COMPLETED);
}