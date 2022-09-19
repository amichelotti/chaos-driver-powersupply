/*
 *	Dant Device Driver
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2022 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <common/powersupply/core/AbstractPowerSupply.h>
#include <boost/regex.hpp>
#include <string>
#include "DanteDD.h"

#define DANTE_INFO INFO_LOG(DanteDD)
#define DANTE_DBG DBG_LOG(DanteDD)
#define DANTE_ERR ERR_LOG(DanteDD)

// GET_PLUGIN_CLASS_DEFINITION
// we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(DanteDD, 1.0.0, chaos::driver::powersupply::DanteDD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::powersupply::DanteDD, http_address / dnsname
                                               : port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

// register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::powersupply::DanteDD)
CLOSE_REGISTER_PLUGIN

using namespace driver::data_import;
using namespace chaos::driver::powersupply;
// default constructor definition
chaos::driver::powersupply::DanteDD::DanteDD() {
  power = NULL;
}

// default descrutcor
chaos::driver::powersupply::DanteDD::~DanteDD() {
}
void chaos::driver::powersupply::DanteDD::driverInit(const chaos::common::data::CDataWrapper &json) throw(chaos::CException) {
  driverInit((const char *)json.getCompliantJSONString().c_str());
}

void chaos::driver::powersupply::DanteDD::driverInit(const char *initParameter) throw(chaos::CException) {
  // check the input parameter
  boost::smatch match;
  std::string   inputStr = initParameter;
  std::string   slaveid;
  DANTE_DBG << "Init  driver initialisation string:\"" << initParameter << "\"" << std::endl;
  dante.driverInit(initParameter);
  // no w accces static DS to get type and other information
  int32_t elementType;
  int     ret = dante.getData("elemType", &elementType, ::driver::data_import::DanteDriver::STATIC);
  if (ret != 0) {
    DANTE_ERR << "Cannot fetch STATIC info";
    // throw chaos::CException(1, "Cannot fetch STATIC info", __PRETTY_FUNCTION__);
  } else {
    protocol      = elementType & 0xF;
    interfaceType = (elementType >> 8) & 0xF;
    polarityType  = (elementType >> 16) & 0xF;
    alarmType     = (elementType >> 24) & 0xF;
  }
  chaos::common::data::CDWUniquePtr res = dante.getDataset();
  if (res.get()) {
    DANTE_DBG << "DATASET:" << res->getJSONString();
  }
}

int DanteDD::setPolarity(int pol, uint32_t timeo_ms) {
  DANTE_DBG << "NOT IMPLEMENTED ";

  return 0;
}

int DanteDD::getPolarity(int *_pol, int *_polsp, uint32_t timeo_ms) {
  int32_t pol = -7, polsp;
  int     ret = dante.getData("outputPolarity", (void *)&pol);

  if (_pol && (ret == 0)) {
    if ((pol > 1) || (pol < -1)) {
      DANTE_ERR << "BAD VALUE FOR POLARITY: " << pol;
      DANTE_DBG << "Attributes:" << dante.getDataset()->getJSONString();

      return -4;
    }
    *_pol = pol;
  } else {
    DANTE_ERR << "Cannot set polarity, err: " << ret;
  }
  if (_polsp) {
    ret += dante.getData("polaritySetting", (void *)&polsp);
    //  DANTE_DBG << "POL SP: "<<polsp;
    *_polsp = polsp;
  }

  return ret;
}

int DanteDD::setCurrentSP(float _current, uint32_t timeo_ms) {
  current = _current;
  return 0;
}

bool DanteDD::isBypass() {
  bool res = false;
  int  ret = dante.getData("byPass", (void *)&res);
  return res;
}

int DanteDD::getCurrentSP(float *_current, uint32_t timeo_ms) {
  *_current   = current;
  double curr = 0;
  int    ret  = dante.getData("currentSetting", (void *)&curr);
  *_current   = curr;

  return ret;
}

int DanteDD::startCurrentRamp(uint32_t timeo_ms) {
  chaos::common::data::CDataWrapper cd;
  cd.addDoubleValue("value", current);
  chaos::common::data::CDWUniquePtr ret = dante.postData("setCurrent", &cd);
  if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR)) {
    chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
    DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
    return -1;
  }
  return 0;
}

int DanteDD::getVoltageOutput(float *volt, uint32_t timeo_ms) {
  double curr = 0;
  int    ret  = dante.getData("outputVolt", (void *)&curr);
  *volt       = curr;

  return ret;
}

int DanteDD::getCurrentOutput(float *_curr, uint32_t timeo_m) {
  double curr = 0;
  int    ret  = dante.getData("outputCurr", (void *)&curr);
  *_curr      = curr;
  return ret;
}

int DanteDD::setCurrentRampSpeed(float asup, float asdown, uint32_t timeo_ms) {
  chaos::common::data::CDataWrapper cd;
  cd.addDoubleValue("value", asup);
  chaos::common::data::CDWUniquePtr ret = dante.postData("setSlope", &cd);
  if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR)) {
    chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
    //DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
    return -1;
  }
  return 0;
}

int DanteDD::resetAlarms(uint64_t alrm, uint32_t timeo_ms) {
  chaos::common::data::CDWUniquePtr ret = dante.postData("resetErrors", NULL);
  if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR)) {
    chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
    DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
    return -1;
  }
  return 0;
}

int DanteDD::getAlarms(uint64_t *alrm, uint32_t timeo_ms) {
  uint32_t curr[4];
  int      ret = dante.getData("faults", (void *)curr);
  *alrm        = *(uint64_t *)curr;  // dont use the other 2 words
  return ret;
}

int DanteDD::shutdown(uint32_t timeo_ms) {
  chaos::common::data::CDWUniquePtr ret = dante.postData("powerOff", NULL);
  if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR)) {
    chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
    DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
    return -1;
  }
  return 0;
}

int DanteDD::standby(uint32_t timeo_ms) {
  chaos::common::data::CDWUniquePtr ret = dante.postData("standby", NULL);
  if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR)) {
    chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
    DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
    return -1;
  }
  return 0;
}
int DanteDD::poweron(uint32_t timeo_ms) {
  chaos::common::data::CDWUniquePtr ret = dante.postData("operational", NULL);
  if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR)) {
    chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
    DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
    return -1;
  }
  return 0;
}
using namespace ::common::powersupply;
int32_t resultState(const int32_t status, const bool remote, const bool trigger, std::string &desc) {
  int32_t           state = 0;
  std::stringstream ss;
  if (remote == false) {
    state |= (int32_t)POWER_SUPPLY_STATE_LOCAL;
    ss << "Local|";
  }
  if (trigger) {
    state |= (int32_t)POWER_SUPPLY_STATE_TRIGGER_ARMED;
    ss << "Trigger|";
  }
  if (status == 1) {
    state |= (int32_t)POWER_SUPPLY_STATE_STANDBY;
    ss << "Stby|";

  } else if (status == 2) {
    state |= (int32_t)POWER_SUPPLY_STATE_ON;
    ss << "Operational|";

  } else if (status == 3) {
    state |= (int32_t)POWER_SUPPLY_STATE_ALARM;
    ss << "Alarm|";
    //    DANTE_DBG << " DANTE ALARM STATE:"<<state<<" status:"<<status;
  }
  desc = ss.str();

  return state;
}
int DanteDD::getState(int *state, std::string &desc, int *statesp, uint32_t timeo_ms) {
  int32_t status, statusSetting;
  bool    remote, trigger;
  int     ret = dante.getData("status", (void *)&status);
  if (ret) {
    DANTE_ERR << "Reading status err:" << ret << " dataset:" << dante.getDataset()->getJSONString();
    return ret;
  }
  ret += dante.getData("statusSetting", (void *)&statusSetting);
  if (ret) {
    DANTE_ERR << "Reading statusSetting err:" << ret << " dataset:" << dante.getDataset()->getJSONString();
    return ret;
  }
  dante.getData("remote", (void *)&remote);
  dante.getData("triggerArmed", (void *)&trigger);
  // DANTE_DBG <<"Attributes:"<<dante.getDataset()->getJSONString();
  *state = resultState(status, remote, trigger, desc);
  // DANTE_DBG <<"state:"<<*state<<" status:"<<status<<" Attributes:"<<dante.getDataset()->getJSONString();

  if (((*state) & (int32_t)POWER_SUPPLY_STATE_ALARM)) {
  //  DANTE_DBG << " DANTE ALARM STATE:" << dante.getDataset()->getJSONString();
  }
  if (statesp) {
    std::string set;
    *statesp = resultState(statusSetting, remote, trigger, set);
    if (((*statesp) & (int32_t)POWER_SUPPLY_STATE_ALARM)) {
      DANTE_DBG << " SETPOINT DANTE ALARM STATE:" << dante.getDataset()->getJSONString();
    }
  }

  /* if(isBypass()){
       return DRV_BYPASS_DEFAULT_CODE;
   }*/
  if (status != statusSetting) {
    return POWER_SUPPLY_OUT_OF_SET;
  }
  return 0;
}

int DanteDD::initPS() {
  return 0;
}

int DanteDD::deinitPS() {
  return 0;
}

int DanteDD::getSWVersion(std::string &version, uint32_t timeo_ms) {
  version = "DanteDD 1.0.0";
  return 0;
}

int DanteDD::getHWVersion(std::string &version, uint32_t timeo_ms) {
  version = protocol2String(protocol);
  return 0;
}

int DanteDD::getCurrentSensibility(float *sens) {
  return 0;
}
int DanteDD::getVoltageSensibility(float *sens) {
  return 0;
}

int DanteDD::setCurrentSensibility(float sens) {
  return 0;
}

int DanteDD::setVoltageSensibility(float sens) {
  return 0;
}
int DanteDD::getMaxMinCurrent(float *max, float *min) {
  return 0;
}

int DanteDD::getMaxMinVoltage(float *max, float *min) {
  return 0;
}
int DanteDD::getAlarmDesc(uint64_t *alarm) {
  return 0;
}

int DanteDD::forceMaxCurrent(float max) {
  return 0;
}

int DanteDD::forceMaxVoltage(float max) {
  return 0;
}
uint64_t DanteDD::getFeatures() {
  if (polarityType == 0) {
    return ::common::powersupply::POWER_SUPPLY_FEAT_BIPOLAR;
  }
  return ::common::powersupply::POWER_SUPPLY_FEAT_MONOPOLAR;
}

chaos::common::data::CDWUniquePtr DanteDD::getDrvProperties() {
  chaos::common::data::CDWUniquePtr ret = dante.getDrvProperties();
  DANTE_DBG << "Props:" << ret->getCompliantJSONString();
  return ret;
}

std::string DanteDD::protocol2String(int32_t t) {
  switch (t) {
    case 0:
      return "SYS8X00";
    case 1:
      return "E642";
    case 2:
      return "Modbus";
    case 3:
      return "Probus";
    case 4:
      return "VSP";
    case 5:
      return "Genesys";
    case 6:
      return "CAENels";
    case 7:
      return "Modbus";
    default:
      return "UNDEFINED";
  }
}
