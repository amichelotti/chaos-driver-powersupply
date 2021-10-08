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
using namespace chaos::cu::control_manager;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;


#define SCCUAPP CUINFO
#define SCCUDBG CUDBG<< "[" << getDeviceID() << "] "
#define SCCUERR CUERR<< "[" << getDeviceID() << "] "

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::powersupply::SCPowerSupplyControlUnit)

/*
 Construct a new CU with an identifier
 */
::driver::powersupply::SCPowerSupplyControlUnit::SCPowerSupplyControlUnit(const string &_control_unit_id,
		const string &_control_unit_param,
		const ControlUnitDriverList &_control_unit_drivers)
:
		//call base constructor
		chaos::cu::control_manager::SCAbstractControlUnit(_control_unit_id,
				_control_unit_param,
				_control_unit_drivers) {
	powersupply_drv = NULL;
}

/*
 Base destructor
 */
::driver::powersupply::SCPowerSupplyControlUnit::~SCPowerSupplyControlUnit() {
	if (powersupply_drv) {
		delete (powersupply_drv);
		powersupply_drv=NULL;
	}
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setSP(const std::string &name, double value, uint32_t size) {
	SCCUDBG << "set SP to " << value;

	return setCurrent(value,true);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setAlarms(const std::string &name, uint64_t value, uint32_t size) {
	SCCUDBG << "set alarms " << value;

	return setAlarms(value,false);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setOff(const std::string &name, bool value, uint32_t size) {
	if (value) {
		SCCUDBG << "set OFF";
		return true;

	}
	SCCUDBG << "set OFF";

	return false;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setStby(const std::string &name, bool value, uint32_t size) {
	if (value) {
		SCCUDBG << "set Standby";
		return powerStandby(true);
	} else {
		SCCUDBG << "set Operational";
		return powerON(true);
	}

	return false;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setPol(const std::string &name, int32_t value, uint32_t size) {
	SCCUDBG << "set polarity:" << value;

	return setPolarity(value,true);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setRampH(const std::string &name, double value, uint32_t size) {
	int err = -1;

	const double *asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampDownRate");
	if (value > 0 && *asdown > 0) {
		SCCUDBG << "set ramp up:" << value<<" ramp down:"<<*asdown;

		err = powersupply_drv->setCurrentRampSpeed(value, *asdown);
	}
	return (err != chaos::ErrorCode::EC_NO_ERROR);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setRampL(const std::string &name, double value, uint32_t size) {
	int err = -1;
	const double *asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampUpRate");
	if (value > 0 && *asup > 0) {
		SCCUDBG << "set ramp down:" << value<<" ramp up:"<<*asup;

		err = powersupply_drv->setCurrentRampSpeed(*asup, value);
	}
	return (err != chaos::ErrorCode::EC_NO_ERROR);
}

/*
 Return the default configuration
 */
void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineActionAndDataset() throw (chaos::CException) {
	//install all command
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSDefault), true,true);
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSMode));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSReset));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSSetSlope));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSSetCurrent));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdSetPolarity));


	// input/output DataSet
	addAttributeToDataSet("stby",
			"force standby",
			DataType::TYPE_BOOLEAN,
			DataType::Bidirectional);
	addAttributeToDataSet("polarity",
			"drive the polarity (for bipolar) -1 negative, 0 open, +1 positive",
			DataType::TYPE_INT32,
			DataType::Bidirectional);

	addAttributeToDataSet("current",
			"setpoint the current",
			DataType::TYPE_DOUBLE,
			DataType::Bidirectional);

	addAttributeToDataSet("voltage",
			"setpoint the voltage",
			DataType::TYPE_DOUBLE,
			DataType::Bidirectional);

	addAttributeToDataSet("alarms",
			"Alarms input clear, output alarm mask",
			DataType::TYPE_INT64,
			DataType::Bidirectional);

	addAttributeToDataSet("rampUpRate",
			"ramp up in A/S or V/s",
			DataType::TYPE_DOUBLE,
			DataType::Bidirectional);

	addAttributeToDataSet("rampDownRate",
			"ramp down in A/S or V/s",
			DataType::TYPE_DOUBLE,
			DataType::Bidirectional);



	addAttributeToDataSet("off",
			"force off (loose control)",
			DataType::TYPE_BOOLEAN,
			DataType::Bidirectional);

	addAttributeToDataSet("triggerArmed",
			"enable triggered acquisition",
			DataType::TYPE_BOOLEAN,
			DataType::Bidirectional);

	addAttributeToDataSet("local",
			"force local (loose control)",
			DataType::TYPE_BOOLEAN,
			DataType::Bidirectional);


	//// CONFIGURATION
	/// power supply configuration
	addAttributeToDataSet("polFromSet",
			"use the sign of set to drive polarity",
			DataType::TYPE_BOOLEAN,
			DataType::Input);

	addAttributeToDataSet("polSwSign",
			"invert the polarity",
			DataType::TYPE_BOOLEAN,
			DataType::Input);

	addAttributeToDataSet("stbyOnPol",
			"force standby on polarity changes",
			DataType::TYPE_BOOLEAN,
			DataType::Input);

	addAttributeToDataSet("zeroOnStby",
			"force zero set on standby on standby",
			DataType::TYPE_BOOLEAN,
			DataType::Input);

	// ===================================== shoul be system
	// addAttributeToDataSet("bypass",
	//         "exclude HW changes",
	//         DataType::TYPE_BOOLEAN,
	//         DataType::Input);

	/// Dataset per analog dataset element properties

/*	addAttributeToDataSet("minimumWorkingValue",
			"minimum accettable working current/voltage value",
			DataType::TYPE_DOUBLE,
			DataType::Input);

	addAttributeToDataSet("maximumWorkingValue",
			"maximum accettable working current/voltage value",
			DataType::TYPE_DOUBLE,
			DataType::Input);
*/
	addAttributeToDataSet("warningThreshold",
			"Defines a tollerance interval within which the readout is considered compliant with the set",
			DataType::TYPE_DOUBLE,
			DataType::Input);

	addAttributeToDataSet("warningThresholdTimeout",
			"the warning condition is generated after the given time in (us)",
			DataType::TYPE_INT32,
			DataType::Input);

	addAttributeToDataSet("setTimeout",
			"timeout for a SET operation(ms)",
			DataType::TYPE_INT32,
			DataType::Input);
	addAttributeToDataSet("getTimeout",
			"timeout for a GET operation(ms)",
			DataType::TYPE_INT32,
			DataType::Input);

	addAttributeToDataSet("driverTimeout",
			"default driver timeout)",
			DataType::TYPE_INT32,
			DataType::Input);
	//addBinaryAttributeAsSubtypeToDataSet("conversionFactor", "Array of double Coefficents used to convert value from the CU actual units and the driver elementary units", chaos::DataType::SUB_TYPE_DOUBLE, 1, chaos::DataType::Input);

/*	addAttributeToDataSet("resolution",
			"Double Minimum meaningful variation of the set respect to the last accepted one",
			DataType::TYPE_DOUBLE,
			DataType::Input);*/

	////////// CONFIGURATION?


	addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, double >(this,
			&::driver::powersupply::SCPowerSupplyControlUnit::setSP,
			"current");
	addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, double >(this,
			&::driver::powersupply::SCPowerSupplyControlUnit::setRampH,
			"rampUpRate");
	addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, double >(this,
			&::driver::powersupply::SCPowerSupplyControlUnit::setRampL,
			"rampDownRate");
	addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, bool >(this,
			&::driver::powersupply::SCPowerSupplyControlUnit::setOff,
			"off");
	addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, bool >(this,
			&::driver::powersupply::SCPowerSupplyControlUnit::setStby,
			"stby");
	addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, int32_t >(this,
			&::driver::powersupply::SCPowerSupplyControlUnit::setPol,
			"polarity");

	addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, uint64_t >(this,
			&::driver::powersupply::SCPowerSupplyControlUnit::setAlarms,
			"alarms");


	addStateVariable(StateVariableTypeAlarmCU,"current_out_of_set",
			"Notify when the 'current' readout drifts respect the 'current' set");

	addStateVariable(StateVariableTypeAlarmCU,"current_value_not_reached",
			"Notify when 'current' readout is not reached");

	addStateVariable(StateVariableTypeAlarmCU,"polarity_out_of_set",
			"Notify when the 'polarity' readout drifts respect the 'polarity' set");

	addStateVariable(StateVariableTypeAlarmCU,"polarity_value_not_reached",
			"Notify when 'polarity' readout is not reached");

	addStateVariable(StateVariableTypeAlarmCU,"stby_out_of_set",
			"Notify when the 'stby' readout drifts respect the 'polarity' set");

	addStateVariable(StateVariableTypeAlarmCU,"stby_value_not_reached",
			"Notify when 'stby' readout is not reached");


	addStateVariable(StateVariableTypeAlarmDEV,"interlock",
			"Notify when an interlock arise");

	addStateVariable(StateVariableTypeAlarmCU,"current_invalid_set",
			"Notify when a 'current' set cannot be done, for limits or mode");

	addStateVariable(StateVariableTypeAlarmCU,"stby_invalid_set",
			"Notify when a 'stby' set cannot be done, for limits or mode");

	addStateVariable(StateVariableTypeAlarmCU,"polarity_invalid_set",
			"Notify when a 'polarity' set cannot be done, for limits or mode");

	addStateVariable(StateVariableTypeAlarmCU,"driver_error",
			"Notify when an error arise from driver");
	addStateVariable(StateVariableTypeAlarmCU,"driver_timeout",
			"Notify when a driver timeout error arise");

	addStateVariable(StateVariableTypeAlarmCU,"communication_failure",
			"Notify when a CU->HW communication error");

	addStateVariable(StateVariableTypeAlarmDEV,"door_open",
			"Notify when a door open");
	addStateVariable(StateVariableTypeAlarmDEV,"over_temp",
			"Notify when a over temperature");

	addStateVariable(StateVariableTypeAlarmDEV,"fuse_fault",
			"Notify when a fuse fault");
	addStateVariable(StateVariableTypeAlarmDEV,"earth_fault",
			"Notify when a earth fault");

	addStateVariable(StateVariableTypeAlarmDEV,"over_voltage",
			"Notify when a over voltage");

	addStateVariable(StateVariableTypeAlarmDEV,"over_current",
			"Notify when a over current");

	addStateVariable(StateVariableTypeAlarmDEV,"communication_failure",
			"Notify when a comunication failure");
	addStateVariable(StateVariableTypeAlarmDEV,"mainunit_failure",
			"Notify when a generic main unit failure");
	addStateVariable(StateVariableTypeAlarmDEV,"external_interlock",
			"Notify when an external interlock");

	addStateVariable(StateVariableTypeAlarmDEV,"card_fault",
			"Notify when a set point card fault");

	addStateVariable(StateVariableTypeAlarmDEV,"cubicle_over_temp",
			"Notify when a cubicle over temperature");

	addStateVariable(StateVariableTypeAlarmDEV,"dcct_fault",
			"Notify when dcct fault");

	addStateVariable(StateVariableTypeAlarmDEV,"active_filter_fuse",
			"Notify when a filter fuse error");

	addStateVariable(StateVariableTypeAlarmDEV,"active_filter_overtemp",
			"Notify when a filter fuse overtemp");

}

void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineCustomAttribute() {
/*  std::string config;
  chaos::common::data::CDWUniquePtr attr;
  
 	chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = getAccessoInstanceByIndex(0);

	attr=power_supply_accessor->getDrvProperties();
  SCCUDBG << "ADDING CUSTOPN:" << attr->getCompliantJSONString();
  setDriverInfo(*attr.get());
  */
}

// Abstract method for the initialization of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitInit() throw (CException) {
	int err = 0;
	int state_id;
	std::string max_range;
	std::string min_range;
	std::string state_str;
	RangeValueInfo attr_info;

	const double *asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampUpRate");
	const double *asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampDownRate");
	//    const bool* s_bypass = getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "bypass");
	//    chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = *s_bypass && (getAccessoInstanceByIndex(1)) ? getAccessoInstanceByIndex(1) : getAccessoInstanceByIndex(0);

	chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = getAccessoInstanceByIndex(0);

	if (power_supply_accessor == NULL) {
		throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
	if (powersupply_drv == NULL) {
		throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}

	if (powersupply_drv->initPS() != 0) {
		throw chaos::CException(-3, "Cannot initialize powersupply " + getCUID(), __FUNCTION__);

	}
	//check mandatory default values
	/*
	 */


	// retrive the attribute description from the device database
	/*
	 * current_sp_attr_info.reset();
	 */
	if ((*asup <= 0) ||(*asdown <= 0)) {
		//throw chaos::CException(-4, "No slop up speed set", __FUNCTION__);
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,"No slop up or down speed set" );

	}
	

	int statesp;
	if ((err=powersupply_drv->getState(&state_id, state_str,&statesp, 30000))< 0) {
		std::stringstream ss;
		ss<<err;
		throw chaos::CException(-6, "Error getting the state of the powersupply, possibily off, err:"+ss.str(), __FUNCTION__);
	}
	//notify change on status_id cached attribute
	getAttributeCache()->setOutputDomainAsChanged();

	if (powersupply_drv->getHWVersion(device_hw, 1000) == 0) {
		SCCUDBG << "hardware found: \"" << device_hw << "\"";
	}


/*	SCCUDBG << "set default slope value up:" << *asup << " down:" << *asdown;
	err = powersupply_drv->setCurrentRampSpeed(*asup, *asdown);
	if ((err != chaos::ErrorCode::EC_NO_ERROR) && (err != chaos::ErrorCode::EC_NODE_OPERATION_NOT_SUPPORTED)) {
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error setting Ramp Speep %1% %2%") % *asup %*asdown) );

		//   throw chaos::CException(-7, "Error setting slope ", __FUNCTION__);
		//TODO: check the  boost::bad_format_string: format-string is ill-formed
		//throw chaos::CException(2, boost::str( boost::format("Error %1 setting the slope in state %2%[%3%]") % err % state_str % state_id), std::string(__FUNCTION__));
	}
*/
}

// Abstract method for the start of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitStart() throw (CException) {

}

// Abstract method for the stop of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitStop() throw (CException) {

}

// Abstract method for the deinit of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitDeinit() throw (CException) {
	SCCUDBG << "deinitializing ";
	powersupply_drv->deinitPS();
	delete powersupply_drv;
	powersupply_drv=NULL;
}

//! restore the control unit to snapshot
#define RESTORE_LDBG SCCUDBG << "[RESTORE-" <<getCUID() << "] "
#define RESTORE_LERR SCCUERR << "[RESTORE-" <<getCUID() << "] "

bool ::driver::powersupply::SCPowerSupplyControlUnit::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache * const snapshot_cache) throw (chaos::CException) {
	//check if in the restore cache we have all information we need
  /*  if (!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("local")) {
        RESTORE_LERR << " missing 'local' to restore";
        return false;
    }*/
    if (!snapshot_cache->getSharedDomain(DOMAIN_INPUT).hasAttribute("polarity")) {
		RESTORE_LERR << " missing 'polarity' to restore";
		return false;
	}
	if (!snapshot_cache->getSharedDomain(DOMAIN_INPUT).hasAttribute("current")) {
		RESTORE_LERR << " missing 'current' to restore";
		return false;
	}

	if (!snapshot_cache->getSharedDomain(DOMAIN_INPUT).hasAttribute("stby")) {
		RESTORE_LERR << " missing 'stby' to restore";
		return false;
	}

    int32_t restore_polarity = (snapshot_cache->getAttributeValue(DOMAIN_INPUT, "polarity"))->getAsVariant();
    double restore_current_sp = (snapshot_cache->getAttributeValue(DOMAIN_INPUT, "current"))->getAsVariant();
    bool restore_stby = (snapshot_cache->getAttributeValue(DOMAIN_INPUT, "stby"))->getAsVariant();

	uint64_t start_restore_time = chaos::common::utility::TimingUtil::getTimeStamp();
	bool cmd_result = true;
	//get actual state
	double *now_current_sp = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "current");
	bool *now_stby = getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "stby");
	bool *now_stby_i = getAttributeCache()->getRWPtr<bool>(DOMAIN_INPUT, "stby");

	int32_t *now_polarity = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");
	int32_t *now_polarity_i = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "polarity");
	uint64_t *alarm=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "alarms");
	RESTORE_LDBG << "Start the restore of the powersupply at:curr:"<<*now_current_sp<<" pol:"<<*now_polarity<<" stby:"<<*now_stby;
	RESTORE_LDBG << "Restore of the powersupply at:curr:"<<restore_current_sp<<" pol:"<<restore_polarity<<" stby:"<<restore_stby;

	if(*alarm!=0){
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,CHAOS_FORMAT("There are active alarms %1% during restore, try to rese",%*alarm ));
		RESTORE_LDBG << " resetting alarms";

		if(!setAlarms(1)){
			RESTORE_LERR << " 2 error resetting alarms";
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("Error resetting alarms active alarms %1% during restore, try to rese",%*alarm ));

			return false;
		}
	}
	int is_bipolar = powersupply_drv->getFeatures()& ::common::powersupply::POWER_SUPPLY_FEAT_BIPOLAR;

	//chec the restore polarity
	bool triggerPolarityRestore=((is_bipolar == 0) && ((*now_polarity != restore_polarity) || (*now_polarity_i!= *now_polarity)));
	bool triggerStbyRestore=(restore_stby != *now_stby) || (*now_stby != *now_stby_i);
	RESTORE_LDBG << "current SP:" << *now_current_sp << "==> " << restore_current_sp;
	RESTORE_LDBG << "current STBY:" << *now_stby << "(input:)"<<*now_stby_i<<" ==>" << restore_stby;

	RESTORE_LDBG << "current POLARITY:" << *now_polarity<< "(input:)"<<*now_polarity_i << " ==>" << restore_polarity;
	if(restore_polarity==0 && restore_current_sp>0){
		RESTORE_LERR<<" incongruent settings polarity =0 and current:"<<restore_current_sp;
		return false;
	}
	// restoring mode
	if (triggerStbyRestore) {
		if (*now_stby == false) {


			// we are in on we should go in stby
			RESTORE_LDBG << "1 set current to 0";
			if (!setCurrent(0)) {
				RESTORE_LERR << " 2 error setting current to 0";
				return false;
			}
			RESTORE_LDBG << "3 going to standby";

			if (!powerStandby()) {
				SCCUERR << " 4 error going in stby";
				return false;
			}
		}
		// now stby = true
		if (triggerStbyRestore) {
			RESTORE_LDBG << "5 changing polarity:" << restore_polarity;
			if (!setPolarity(restore_polarity)) {
				RESTORE_LERR << "6 error restoring polariy to " << restore_polarity;
				return false;
			}
		}
		if(restore_stby==false){
			if (powerON() == false) {
				RESTORE_LERR << " 10 error restoring poweron";
				return false;

			}
			RESTORE_LDBG << "11 restoring current:" << restore_current_sp;

			if (!setCurrent(restore_current_sp)) {
				RESTORE_LERR << "12 error restoring current to " << restore_current_sp;
				return false;
			}

		}
		uint64_t restore_duration_in_ms = chaos::common::utility::TimingUtil::getTimeStamp() - start_restore_time;
		RESTORE_LDBG << "[metric] Restore successfully achieved in " << restore_duration_in_ms << " milliseconds";
		return true;
	}


	//handle bipolar

	///
	if (triggerStbyRestore) {
		//we need to change the polarity
		RESTORE_LDBG << "Change the polarity from:" << *now_polarity << "(input:"<<*now_polarity_i<<") to:" << restore_polarity;

		//put in standby
		RESTORE_LDBG << "Put powersupply at setpoint 0";
		if (setCurrent(0.0)) {
			RESTORE_LDBG << "going to standby";

			if (powerStandby()) {
				RESTORE_LDBG << "Powersupply is gone in standby";
			} else {
				SCCUERR << "Power supply is not gone in standby";
				return false;
			}
		} else {
			RESTORE_LERR << "Power supply is not gone to 0 ampere";
			return false;
		}
		//set the polarity
		RESTORE_LDBG << "Apply new polarity:" << restore_polarity;
		if (!setPolarity(restore_polarity)) {
			RESTORE_LERR << "Power supply is not gone to restore polarity" << restore_polarity;
			return false;
		}
		if (restore_stby == false) {
			RESTORE_LDBG << "going power on";

			if (powerON() == false) {
				RESTORE_LERR << " error restoring poweron";
				return false;
			}
		}
	}
	RESTORE_LDBG << "setting current:" << restore_current_sp;

	if (!setCurrent(restore_current_sp)) {
		SCCUERR << "## error restoring current:" << restore_current_sp;
		return false;
	}

	uint64_t restore_duration_in_ms = chaos::common::utility::TimingUtil::getTimeStamp() - start_restore_time;
	RESTORE_LDBG << "[metric] Restore successfully achieved in " << restore_duration_in_ms << " milliseconds";
	return true;
}
//-----------utility methdo for the restore operation---------

bool ::driver::powersupply::SCPowerSupplyControlUnit::powerON(bool sync) {
	uint64_t cmd_id;
	bool result = true;
	ChaosUniquePtr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addInt32Value(CMD_PS_MODE_TYPE, 1);
	//send command
	if(getState()!=chaos::CUStateKey::START){
		SCCUERR << "## not in start ..";

		return false;
	}
	submitBatchCommand(CMD_PS_MODE_ALIAS,
			cmd_pack.release(),
			cmd_id,
			0,
			50,
			SubmissionRuleType::SUBMIT_NORMAL);
	if (sync) {
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
	if(getState()!=chaos::CUStateKey::START){
		return true;
	}
	submitBatchCommand(CMD_PS_MODE_ALIAS,
			cmd_pack.release(),
			cmd_id,
			0,
			50,
			SubmissionRuleType::SUBMIT_NORMAL);
	if (sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setPolarity(int polarity,
		bool sync) {
	uint64_t cmd_id;
	bool result = true;
	ChaosUniquePtr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addInt32Value(CMD_PS_SET_POLARITY_VALUE, polarity);
	//send command
	submitBatchCommand(CMD_PS_SET_POLARITY_ALIAS,
			cmd_pack.release(),
			cmd_id,
			0,
			50,
			SubmissionRuleType::SUBMIT_NORMAL);
	if (sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setAlarms(uint64_t alarms,
		bool sync) {
	uint64_t cmd_id;
	bool result = true;
	ChaosUniquePtr<CDataWrapper>cmd_pack(new CDataWrapper());
	cmd_pack->addInt64Value("alarms", (uint64_t) alarms);
	//send command
	submitBatchCommand(CMD_PS_RESET_ALIAS,
			cmd_pack.release(),
			cmd_id,
			0,
			50,
			SubmissionRuleType::SUBMIT_NORMAL);
	if (sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setCurrent(double current_set_point,
		bool sync) {
	uint64_t cmd_id;
	bool result = true;
	ChaosUniquePtr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addDoubleValue(CMD_PS_SET_CURRENT, current_set_point);
	if(getState()!=chaos::CUStateKey::START){
		return true;
	}
	//send command
	submitBatchCommand(CMD_PS_SET_CURRENT_ALIAS,
			cmd_pack.release(),
			cmd_id,
			0,
			50,
			SubmissionRuleType::SUBMIT_NORMAL);
	if (sync) {
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
	ChaosUniquePtr<CDataWrapper> cmd_pack(new CDataWrapper());
	cmd_pack->addDoubleValue(CMD_PS_SET_SLOPE_UP, sup);
	cmd_pack->addDoubleValue(CMD_PS_SET_SLOPE_DOWN, sdown);
	//send command
	submitBatchCommand(CMD_PS_SET_SLOPE_ALIAS,
			cmd_pack.release(),
			cmd_id,
			0,
			50,
			SubmissionRuleType::SUBMIT_NORMAL);
	if (sync) {
		//! whait for the current command id to finisch
		result = whaitOnCommandID(cmd_id);
	}
	return result;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::whaitOnCommandID(uint64_t cmd_id,int timeo_ms) {
	ChaosUniquePtr<chaos::common::batch_command::CommandState> cmd_state;
	uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();
	do {
		cmd_state = getStateForCommandID(cmd_id);
		if (!cmd_state.get()) {
			SCCUERR<<"cannot get command state";
			
			return true;
		}
		switch (cmd_state->last_event) {
		case BatchCommandEventType::EVT_QUEUED:
			SCCUDBG << cmd_id << " -> QUEUED";
			break;
		case BatchCommandEventType::EVT_RUNNING:
			SCCUDBG << cmd_id << " -> RUNNING";
			break;
		case BatchCommandEventType::EVT_WAITING:
			SCCUDBG << cmd_id << " -> WAITING";
			break;
		case BatchCommandEventType::EVT_PAUSED:
			SCCUDBG << cmd_id << " -> PAUSED";
			break;
		case BatchCommandEventType::EVT_KILLED:
			SCCUDBG << cmd_id << " -> KILLED";
			break;
		case BatchCommandEventType::EVT_COMPLETED:
			SCCUDBG << cmd_id << " -> COMPLETED";
			break;
		case BatchCommandEventType::EVT_FAULT:
			SCCUDBG << cmd_id << " -> FAULT";
			break;
		}
		//whait some times
		usleep(500000);
	} while ((cmd_state->last_event != BatchCommandEventType::EVT_COMPLETED) &&
			(cmd_state->last_event != BatchCommandEventType::EVT_FAULT) &&
			(cmd_state->last_event != BatchCommandEventType::EVT_KILLED) &&((chaos::common::utility::TimingUtil::getTimeStamp()-now)<timeo_ms));

	return (cmd_state.get() &&
			cmd_state->last_event == BatchCommandEventType::EVT_COMPLETED);
}
