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
  }
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setSP(const std::string &name,double value,uint32_t size){
    SCCUAPP << "set SP to " << value;

	return setCurrent(value);
}
bool ::driver::powersupply::SCPowerSupplyControlUnit::setAlarms(const std::string &name,long long value,uint32_t size){
    SCCUAPP << "set alarms " << value;
	if(powersupply_drv->resetAlarms(value) != 0) {
		return false;
	}

	return true;
}
bool  ::driver::powersupply::SCPowerSupplyControlUnit::setOn(const std::string &name,bool value,uint32_t size){
    if(value){
        SCCUAPP << "set ON";
        return powerON();

    }
    SCCUAPP << "set OFF";
    
    return false;
}

bool  ::driver::powersupply::SCPowerSupplyControlUnit::setStby(const std::string &name,bool value,uint32_t size){
    if(value){
        SCCUAPP << "set Standby";
        return powerStandby();
    }
    
    return false;
}
bool  ::driver::powersupply::SCPowerSupplyControlUnit::setPol(const std::string &name,int32_t value,uint32_t size){
    SCCUAPP << "set polarity:"<<value;

	return setPolarity(value);
}
bool ::driver::powersupply::SCPowerSupplyControlUnit::setRampH(const std::string &name,double value,uint32_t size){
	int err=-1;

	  const double *asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampDownRate");
	  if(value>0 && *asdown>0){
		    SCCUAPP << "set ramp up:"<<value;

		  err = powersupply_drv->setCurrentRampSpeed(value, *asdown);
	  }
	 return (err!=chaos::ErrorCode::EC_NO_ERROR);
}
bool ::driver::powersupply::SCPowerSupplyControlUnit::setRampL(const std::string &name,double value,uint32_t size){
	int err= -1;
	  const double *asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampUpRate");
	  if(value>0 && *asup>0){
		    SCCUAPP << "set ramp down:"<<value;

		  err = powersupply_drv->setCurrentRampSpeed(*asup, value);
	  }
	 return (err!=chaos::ErrorCode::EC_NO_ERROR);
}
/*
 Return the default configuration
 */
void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineActionAndDataset() throw(chaos::CException) {
  //install all command
  installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSDefault), true);
  installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSMode));
  installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSReset));
  installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSSetSlope));
  installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSSetCurrent));
  installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdSetPolarity));
    
    
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
    addAttributeToDataSet("bypass",
                          "exclude HW changes",
                          DataType::TYPE_BOOLEAN,
                          DataType::Input);
    
    /// Dataset per analog dataset element properties
    addAttributeToDataSet("unit",
                          "current/voltage units",
                          DataType::TYPE_STRING,
                          DataType::Input);
    
    addAttributeToDataSet("scale",
                          "current/voltage scale",
                          DataType::TYPE_INT32,
                          DataType::Input);
    
    addAttributeToDataSet("minimumWorkingValue",
                          "minimum accettable working current/voltage value",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("maximumWorkingValue",
                          "maximum accettable working current/voltage value",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("warningThreshold",
                          "Defines a tollerance interval within which the readout is considered compliant with the set",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("warningThresholdTimeout",
                          "the warning condition is generated after the given time in (us)",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addBinaryAttributeAsSubtypeToDataSet("conversionFactor","Array of double Coefficents used to convert value from the CU actual units and the driver elementary units",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Input);

    addAttributeToDataSet("resolution",
                          "Double Minimum meaningful variation of the set respect to the last accepted one",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);

    //////////
    
    // input/output DataSet
    addAttributeToDataSet("current",
                          "setpoint the current",
                          DataType::TYPE_DOUBLE,
                          DataType::Bidirectional);
    
    addAttributeToDataSet("voltage",
                          "setpoint the voltage",
                          DataType::TYPE_DOUBLE,
                          DataType::Bidirectional);
    
    addAttributeToDataSet("rampUpRate",
                          "ramp up in A/S or V/s",
                          DataType::TYPE_DOUBLE,
                          DataType::Bidirectional);
    
    addAttributeToDataSet("rampDownRate",
                          "ramp down in A/S or V/s",
                          DataType::TYPE_DOUBLE,
                          DataType::Bidirectional);
    
    addAttributeToDataSet("stby",
                          "force standby",
                          DataType::TYPE_BOOLEAN,
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
    
    
    addAttributeToDataSet("polarity",
                          "drive the polarity (for bipolar) -1 negative, 0 open, +1 positive",
                          DataType::TYPE_INT32,
                          DataType::Bidirectional);
    
    
  addAttributeToDataSet("alarms",
                        "Alarms input clear, output alarm mask",
                        DataType::TYPE_INT64,
                        DataType::Bidirectional);

    
    
  

  addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, double >(this,
                                                                    &::driver::powersupply::SCPowerSupplyControlUnit::setSP,
                                                                    "current_voltage");
  addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, double >(this,
                                                                      &::driver::powersupply::SCPowerSupplyControlUnit::setRampH,
                                                                      "rampUpRate");
  addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, double >(this,
                                                                      &::driver::powersupply::SCPowerSupplyControlUnit::setRampL,
                                                                      "rampDownRate");
  addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, bool >(this,
                                                                      &::driver::powersupply::SCPowerSupplyControlUnit::setOn,
                                                                      "on");
  addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, bool >(this,
                                                                      &::driver::powersupply::SCPowerSupplyControlUnit::setStby,
                                                                      "stby");
  addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, int32_t >(this,
                                                                        &::driver::powersupply::SCPowerSupplyControlUnit::setPol,
                                                                        "polarity");

  addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, long long  >(this,
                                                                         &::driver::powersupply::SCPowerSupplyControlUnit::setAlarms,
                                                                         "alarms");
}

void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineCustomAttribute() {

}

// Abstract method for the initialization of the control unit
void ::driver::powersupply::SCPowerSupplyControlUnit::unitInit() throw(CException) {
  SCCUAPP << "Starting restore";

  int err = 0;
  int state_id;
  std::string max_range;
  std::string min_range;
  std::string state_str;
  RangeValueInfo attr_info;

  const double *asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampUpRate");
  const double *asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampDownRate");
  int32_t *status_id = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "status_id");


  chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = getAccessoInstanceByIndex(0);
  if (power_supply_accessor == NULL) {
    throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
  }
  powersupply_drv = new chaos::driver::powersupply::ChaosPowerSupplyInterface(power_supply_accessor);
  if (powersupply_drv == NULL) {
    throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
  }

  if (powersupply_drv->init() != 0) {
    throw chaos::CException(-3, "Cannot initialize powersupply " + control_unit_instance, __FUNCTION__);

  }
  //check mandatory default values
  /*
   */
  SCCUAPP << "check mandatory default values";
  getAttributeRangeValueInfo(std::string("currentSP"), attr_info);

  // REQUIRE MIN MAX SET IN THE MDS
  if (attr_info.maxRange.size()) {
    SCCUAPP << "max_current max=" << (max_range = attr_info.maxRange);
        SCCUAPP << "min_current min=" << (min_range = attr_info.minRange);

  }

  // REQUIRE MIN MAX SET IN THE MDS
  if (attr_info.minRange.size()) {
        SCCUAPP << "min_current min=" << (min_range = attr_info.minRange);

  }


  // retrive the attribute description from the device database
  /*
   * current_sp_attr_info.reset();
   */
  if (*asup <= 0) {
    throw chaos::CException(-4, "No slop up speed set", __FUNCTION__);
  }
  if (*asdown <= 0) {
    throw chaos::CException(-5, "No slop down speed set", __FUNCTION__);
  }


  if (powersupply_drv->getState(&state_id, state_str, 30000) != 0) {
    throw chaos::CException(-6, "Error getting the state of the powersupply, possibily off", __FUNCTION__);
  }
  *status_id = state_id;
  //notify change on status_id cached attribute
  getAttributeCache()->setOutputDomainAsChanged();

  if (powersupply_drv->getHWVersion(device_hw, 1000) == 0) {
    SCCUAPP << "hardware found: \"" << device_hw << "\"";
  }


  SCCUAPP << "set default slope value up:" << *asup << " down:" << *asdown;
  err = powersupply_drv->setCurrentRampSpeed(*asup, *asdown);
  if ((err != chaos::ErrorCode::EC_NO_ERROR) && (err != chaos::ErrorCode::EC_NODE_OPERATION_NOT_SUPPORTED)) {
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
    SCCUAPP << "deinitializing ";
    powersupply_drv->deinit();
    
}

//! restore the control unit to snapshot
#define RESTORE_LAPP SCCUAPP << "[RESTORE-" <<getCUID() << "] "
#define RESTORE_LERR SCCUERR << "[RESTORE-" <<getCUID() << "] "
bool ::driver::powersupply::SCPowerSupplyControlUnit::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache *const snapshot_cache) throw(chaos::CException) {
  RESTORE_LAPP << "Check if restore cache has the needed data";
  //check if in the restore cache we have all information we need
  if (!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("status_id")) return false;
  if (!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("polarity")) return false;
  if (!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("current_voltage")) return false;

  RESTORE_LAPP << "Start the restore of the powersupply";
  uint64_t start_restore_time = chaos::common::utility::TimingUtil::getTimeStamp();
  try {
    bool cmd_result = true;
    //get actual state
    double *now_current_sp = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "current_voltage");
    bool now_stby = *getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "stby");
    int32_t *now_polarity = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");

    //chec the restore polarity
    int32_t restore_polarity = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "polarity")->getValuePtr<int32_t>();
    double restore_current_sp = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "current_voltage")->getValuePtr<double>();
    //handle bipolar
    int is_bipolar=powersupply_drv->getFeatures()& ::common::powersupply::POWER_SUPPLY_FEAT_BIPOLAR;
   
    ///
    if ((*now_polarity != restore_polarity)&&(is_bipolar==0)) {
      //we need to change the polarity
      RESTORE_LAPP << "Change the polarity from:" << *now_polarity << " to:" << restore_polarity;

      //put in standby
      RESTORE_LAPP << "Put powersupply at setpoint 0";
      if (setCurrent(0.0)) {
        usleep(100000);
        RESTORE_LAPP << "Start the restore of the powersupply";
        if (powerStandby()) {
          usleep(100000);
          RESTORE_LAPP << "Powersupply is gone in standby";
        } else {
          LOG_AND_TROW(RESTORE_LERR, 2, "Power supply is not gone in standby");
        }
      } else {
        LOG_AND_TROW(RESTORE_LERR, 1, "Power supply is not gone to 0 ampere");
      }

      //set the polarity
      RESTORE_LAPP << "Apply new polarity";
      if (!setPolarity(restore_polarity)) {
        LOG_AND_TROW_FORMATTED(RESTORE_LERR, 3, "Power supply is not gone to restore polarity %1%", %restore_polarity);
      }
      usleep(100000);
    }
    
    bool isStby = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "stby")->getValuePtr<bool>();
    bool islocal = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "local")->getValuePtr<bool>();

      
    if (now_stby != isStby) {
        if(isStby){
            if (setCurrent(0.0)) {
                if (powerStandby()) {
                    RESTORE_LAPP << "Powersupply is gone in standby";
            }
            }
        } else {
            RESTORE_LAPP << "Put powersupply in on state to restore his status";
            powerON();
        }
      
    usleep(100000);
    RESTORE_LAPP << "Apply new setpoint " << restore_current_sp;
    if (!setCurrent(restore_current_sp)) {
      LOG_AND_TROW_FORMATTED(RESTORE_LERR,
                             6,
                             "Power supply is not gone to restore 'current setpoint %1%' state",
                             %restore_current_sp);
    }
    uint64_t restore_duration_in_ms = chaos::common::utility::TimingUtil::getTimeStamp() - start_restore_time;
    RESTORE_LAPP << "[metric] Restore successfully achieved in " << restore_duration_in_ms << " milliseconds";
  } catch (CException &ex) {
    uint64_t restore_duration_in_ms = chaos::common::utility::TimingUtil::getTimeStamp() - start_restore_time;
    RESTORE_LAPP << "[metric] Restore has fault in " << restore_duration_in_ms << " milliseconds";
    throw ex;
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
  std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
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
  do {
    cmd_state = getStateForCommandID(cmd_id);
    if (!cmd_state.get()) break;

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
    //whait some times
    usleep(500000);
  } while (cmd_state->last_event != BatchCommandEventType::EVT_COMPLETED &&
      cmd_state->last_event != BatchCommandEventType::EVT_FAULT &&
      cmd_state->last_event != BatchCommandEventType::EVT_KILLED);
  return (cmd_state.get() &&
      cmd_state->last_event == BatchCommandEventType::EVT_COMPLETED);
}
