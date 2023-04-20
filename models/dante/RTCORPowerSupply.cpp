/*
 *	RTCORPowerSupply
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2021 INFN, National Institute of Nuclear Physics
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

#include "RTCORPowerSupply.h"
#include <common/powersupply/powersupply.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos;

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace ::common::powersupply;
using namespace ::driver::powersupply;
#define SCCUAPP CUINFO
#define SCCUDBG CUDBG << "[" << getDeviceID() << "] "
#define SCCUERR CUERR << "[" << getDeviceID() << "] "

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::powersupply::RTCORPowerSupply)

/*
 Construct a new CU with an identifier
 */

static int32_t resultState(const int32_t status, const bool remote, const bool trigger, std::string &desc) {
  int32_t           state = 0;
  std::stringstream ss;
  if (remote == false) {
    state |= (int32_t)::common::powersupply::POWER_SUPPLY_STATE_LOCAL;
    ss << "Local|";
  }
  if (trigger) {
    state |= (int32_t)::common::powersupply::POWER_SUPPLY_STATE_TRIGGER_ARMED;
    ss << "Trigger|";
  }
  if ((status == 0)) { 
    state |= (int32_t)::common::powersupply::POWER_SUPPLY_STATE_STANDBY;
    ss << "Stby|";

  } else if ((status == 1)) {
    state |= (int32_t)::common::powersupply::POWER_SUPPLY_STATE_ON;
    ss << "Operational|";

  } else if ((status == 2)){ 
    state |= (int32_t)::common::powersupply::POWER_SUPPLY_STATE_ALARM;
    ss << "Alarm|";
    //    DANTE_DBG << " DANTE ALARM STATE:"<<state<<" status:"<<status;

  } else if (status == 3) {
    state |= (int32_t)::common::powersupply::POWER_SUPPLY_STATE_ERROR;
    ss << "Error|";
    //    DANTE_DBG << " DANTE ALARM STATE:"<<state<<" status:"<<status;

  }  else {
    state = (int32_t)::common::powersupply::POWER_SUPPLY_STATE_UKN;
    ss << "Uknown|";
  }
  desc = ss.str();

  return state;
}
::driver::powersupply::RTCORPowerSupply::RTCORPowerSupply(const string &               _control_unit_id,
                                                          const string &               _control_unit_param,
                                                          const ControlUnitDriverList &_control_unit_drivers)
    : RTAbstractControlUnit(_control_unit_id,
                            _control_unit_param,
                            _control_unit_drivers) {
  driver.driverInit(_control_unit_param.c_str());
}

/*
 Base destructor
 */
::driver::powersupply::RTCORPowerSupply::~RTCORPowerSupply() {
}

/*
 Return the default configuration
 */
void ::driver::powersupply::RTCORPowerSupply::unitDefineActionAndDataset()  {
  //install all command

  // input/output DataSet
  addAttributeToDataSet("stby",
                        "force standby",
                        DataType::TYPE_BOOLEAN,
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
  addAttributeToDataSet("alarms2",
                        "Alarms supplemental",
                        DataType::TYPE_INT64,
                        DataType::Output);

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
 
  addStateVariable(StateVariableTypeAlarmCU, "fetch_error", "Fetch error");

  addStateVariable(StateVariableTypeAlarmDEV, "interlock", "Notify when an interlock arise alarms !=0");

  addStateVariable(StateVariableTypeAlarmDEV, "bad_state", "Notify when a bad state state=4");
  addStateVariable(StateVariableTypeAlarmDEV, "unknown_state", "Notify when a bad state state>4");
 // addStateVariable(StateVariableTypeAlarmDEV, "shutdown_state", "Notify when shutdown state state=0");

  addStateVariable(StateVariableTypeAlarmDEV, "faulty_state", "Notify when a state=3 arise");

  addStateVariable(StateVariableTypeAlarmDEV, "door_open", "Notify when a door open");
  addStateVariable(StateVariableTypeAlarmDEV, "over_temp", "Notify when a over temperature");

  addStateVariable(StateVariableTypeAlarmDEV, "fuse_fault", "Notify when a fuse fault");
  addStateVariable(StateVariableTypeAlarmDEV, "earth_fault", "Notify when a earth fault");

  addStateVariable(StateVariableTypeAlarmDEV, "over_voltage", "Notify when a over voltage");

  addStateVariable(StateVariableTypeAlarmDEV, "over_current", "Notify when a over current");

  addStateVariable(StateVariableTypeAlarmDEV, "communication_failure", "Notify when a comunication failure");
  addStateVariable(StateVariableTypeAlarmDEV, "mainunit_failure", "Notify when a generic main unit failure");
  addStateVariable(StateVariableTypeAlarmDEV, "external_interlock", "Notify when an external interlock");

  addStateVariable(StateVariableTypeAlarmDEV, "card_fault", "Notify when a set point card fault");

  addStateVariable(StateVariableTypeAlarmDEV, "cubicle_over_temp", "Notify when a cubicle over temperature");

  addStateVariable(StateVariableTypeAlarmDEV, "dcct_fault", "Notify when dcct fault");

  addStateVariable(StateVariableTypeAlarmDEV, "active_filter_fuse", "Notify when a filter fuse error");

  addStateVariable(StateVariableTypeAlarmDEV, "active_filter_overtemp", "Notify when a filter fuse overtemp");
}

void ::driver::powersupply::RTCORPowerSupply::unitDefineCustomAttribute() {
  /*  std::string config;
  chaos::common::data::CDWUniquePtr attr;
  
 	chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = getAccessoInstanceByIndex(0);

	attr=power_supply_accessor->getDrvProperties();
  SCCUDBG << "ADDING CUSTOPN:" << attr->getCompliantJSONString();
  setDriverInfo(*attr.get());
  */
}

// Abstract method for the initialization of the control unit

void ::driver::powersupply::RTCORPowerSupply::unitInit() {
  in.reset();
  out.reset();
  in.addDoubleValue("currentSetting", 0);
  in.addDoubleValue("slewRateSetting", 0);
  in.addInt32Value("statusSetting", 0);

  out.addDoubleValue("outputCurr", 0);
  out.addDoubleValue("outputVolt", 0);
  out.addInt32Value("status", 0);
  out.addInt32Value("statusReadout", 0);

  out.addBoolValue("onLine", false);
  out.addBoolValue("busy", false);
  out.addBoolValue("byPass",false);

  out.addInt32Value("triggerArmed", 0);

  out.addInt32Value("slewRateSetting", 0);
  out.addDoubleValue("slewRateReadout", 0);
  alarms[0]=0;
  alarms[1]=0;

  if ((driver.getData(in) != 0) || (driver.getData(out) != 0)) {
    setStateVariableSeverity(StateVariableTypeAlarmCU, "fetch_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
  }
}

// Abstract method for the start of the control unit

void ::driver::powersupply::RTCORPowerSupply::unitStart() {
  if ((driver.getData(in) != 0) || (driver.getData(out) != 0)) {
    setStateVariableSeverity(StateVariableTypeAlarmCU, "fetch_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
  } else {
    acquireIn();
    acquireOut();
    getAttributeCache()->setOutputDomainAsChanged();
    getAttributeCache()->setInputDomainAsChanged();

    pushInputDataset();
    pushOutputDataset();
  }
}

// Abstract method for the stop of the control unit

void ::driver::powersupply::RTCORPowerSupply::unitStop() {
}

// Abstract method for the deinit of the control unit

void ::driver::powersupply::RTCORPowerSupply::unitDeinit() {
}
chaos::common::data::CDWUniquePtr RTCORPowerSupply::getProperty(chaos::common::data::CDWUniquePtr d) {
  return driver.getDataset();
}
chaos::common::data::CDWUniquePtr RTCORPowerSupply::setProperty(chaos::common::data::CDWUniquePtr d) {
  return chaos::common::data::CDWUniquePtr();
}

void  RTCORPowerSupply::setFlags(){
  std::string desc;
  int32_t state = resultState(out.getInt32Value("statusReadout"), out.getBoolValue("onLine"), out.getBoolValue("triggerArmed"), desc);

  if(out.getBoolValue("byPass")){
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "faulty_state", chaos::common::alarm::MultiSeverityAlarmLevelClear);
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "bad_state", chaos::common::alarm::MultiSeverityAlarmLevelClear);
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "interlock", chaos::common::alarm::MultiSeverityAlarmLevelClear);
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "unknown_state", chaos::common::alarm::MultiSeverityAlarmLevelClear);
    //setStateVariableSeverity(StateVariableTypeAlarmDEV, "shutdown_state", chaos::common::alarm::MultiSeverityAlarmLevelClear);

    setStateVariableSeverity(StateVariableTypeAlarmCU, "current_out_of_set",chaos::common::alarm::MultiSeverityAlarmLevelClear);
    setStateVariableSeverity(StateVariableTypeAlarmCU, "stby_out_of_set",chaos::common::alarm::MultiSeverityAlarmLevelClear);
    setBypassFlag(true);
   //CUDBG<<" BYPASS ENABLED";
    return;
  }
    setBypassFlag(false);

  if (state & ::common::powersupply::POWER_SUPPLY_STATE_ALARM) {
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "faulty_state", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
  } else {
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "faulty_state", chaos::common::alarm::MultiSeverityAlarmLevelClear);
  }

  if (state & ::common::powersupply::POWER_SUPPLY_STATE_ERROR) {
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "bad_state", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
  } else {
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "bad_state", chaos::common::alarm::MultiSeverityAlarmLevelClear);
  }
  if (state & ::common::powersupply::POWER_SUPPLY_STATE_SHUTDOWN) {
      getAttributeCache()->setOutputAttributeValue("off", true);
      setStateVariableSeverity(StateVariableTypeAlarmCU, "current_out_of_set",chaos::common::alarm::MultiSeverityAlarmLevelClear);
      setStateVariableSeverity(StateVariableTypeAlarmCU, "stby_out_of_set",chaos::common::alarm::MultiSeverityAlarmLevelClear);

   // setStateVariableSeverity(StateVariableTypeAlarmDEV, "shutdown_state", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
  } else {
      getAttributeCache()->setOutputAttributeValue("off", false);
  }
  if (state == ::common::powersupply::POWER_SUPPLY_STATE_UKN) {
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "unknown_state", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
  } else {
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "unknown_state", chaos::common::alarm::MultiSeverityAlarmLevelClear);
  }
  
 /* if(alarms[0]){
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "interlock", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
  } else {
    setStateVariableSeverity(StateVariableTypeAlarmDEV, "interlock", chaos::common::alarm::MultiSeverityAlarmLevelClear);

  }*/
}

int RTCORPowerSupply::acquireOut() {
  std::string desc;

  int32_t state = resultState(out.getInt32Value("status"), out.getBoolValue("onLine"), out.getBoolValue("triggerArmed"), desc);
  getAttributeCache()->setOutputAttributeValue("voltage", out.getDoubleValue("outputVolt"));
  getAttributeCache()->setOutputAttributeValue("current", out.getDoubleValue("outputCurr"));
  getAttributeCache()->setOutputAttributeValue("rampUpRate", out.getDoubleValue("slewRateReadout"));
  getAttributeCache()->setOutputAttributeValue("rampDownRate", out.getDoubleValue("slewRateReadout"));
  bool stby=((state & ::common::powersupply::POWER_SUPPLY_STATE_STANDBY) ? true : false);
  getAttributeCache()->setOutputAttributeValue("stby", stby);
  if(stby==true){
    setReadoutCheck("current",false); //disable auto check
    setStateVariableSeverity(StateVariableTypeAlarmCU, "current_out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);

    //setStateMask("current_out_of_set",true);
  } else {
    // setStateMask("current_out_of_set",false);
    setReadoutCheck("current",true);


  }
  getAttributeCache()->setOutputAttributeValue("local", ((state & ::common::powersupply::POWER_SUPPLY_STATE_LOCAL) ? true : false));
  
  setBusyFlag(out.getBoolValue("busy"));
  driver.getData("faults", (void *)alarms);
  getAttributeCache()->setOutputAttributeValue("alarms", alarms[0]);
  getAttributeCache()->setOutputAttributeValue("alarms2", alarms[1]);
 // getAttributeCache()->setOutputDomainAsChanged();
  return state;
}
int RTCORPowerSupply::acquireIn() {
  std::string desc;

  getAttributeCache()->setInputAttributeValue("current", in.getDoubleValue("currentSetting"));
  getAttributeCache()->setInputAttributeValue("rampUpRate", in.getDoubleValue("slewRateSetting"));
  getAttributeCache()->setInputAttributeValue("rampDownRate", in.getDoubleValue("slewRateSetting"));
  int32_t statesp = resultState(in.getInt32Value("statusSetting"), out.getBoolValue("onLine"), out.getBoolValue("triggerArmed"), desc);
  getAttributeCache()->setInputAttributeValue("stby", ((statesp & ::common::powersupply::POWER_SUPPLY_STATE_STANDBY) ? true : false));
  getAttributeCache()->setInputAttributeValue("local", ((statesp & ::common::powersupply::POWER_SUPPLY_STATE_LOCAL) ? true : false));
  return statesp;
}
void ::driver::powersupply::RTCORPowerSupply::unitRun()  {
  setStateVariableSeverity(StateVariableTypeAlarmCU, "fetch_error", chaos::common::alarm::MultiSeverityAlarmLevelClear);

  try {
    if ((driver.getData(in) != 0) || (driver.getData(out) != 0)) {
      setStateVariableSeverity(StateVariableTypeAlarmCU, "fetch_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
    } else {
      int istate=acquireIn();
      int outstate=acquireOut();
      
      if ((in != pin)) {
        SCCUDBG << "INPUT CHANGED prev:" << pin.getCompliantJSONString();
        SCCUDBG << "INPUT CHANGED curr:" << in.getCompliantJSONString();
        pin.reset();

        in.copyAllTo(pin);
        getAttributeCache()->setInputDomainAsChanged();
        pushInputDataset();
      }
      if (out != pout) {
        SCCUDBG << "OUTPUT CHANGED prev:" << pout.getCompliantJSONString();
        SCCUDBG << "OUTPUT CHANGED curr:" << out.getCompliantJSONString();
        pout.reset();
        out.copyAllTo(pout);
        //acquireOut();
        getAttributeCache()->setOutputDomainAsChanged();
      }
        setFlags(); 
    }
    //SCCUDBG << "OUTPUT:"<<out.getCompliantJSONString();

   
  } catch (chaos::CException &e) {
    setStateVariableSeverity(StateVariableTypeAlarmCU, "fetch_error", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
    SCCUERR << "Fetch error:" << e.what();
  }

  //getAttributeCache()->setOutputDomainAsChanged();
  // getAttributeCache()->setInputDomainAsChanged();
}