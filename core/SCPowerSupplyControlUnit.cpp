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


#define SCCUAPP INFO_LOG(SCPowerSupplyControlUnit) << "[" << getDeviceID() << "] "
#define SCCUDBG DBG_LOG(SCPowerSupplyControlUnit) << "[" << getDeviceID() << "] "
#define SCCUERR ERR_LOG(SCPowerSupplyControlUnit) << "[" << getDeviceID() << "] "

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

bool ::driver::powersupply::SCPowerSupplyControlUnit::setSP(const std::string &name, double value, uint32_t size) {
    SCCUAPP << "set SP to " << value;

    return setCurrent(value,true);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setAlarms(const std::string &name, long long value, uint32_t size) {
    SCCUAPP << "set alarms " << value;

    return setAlarms(value,false);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setOff(const std::string &name, bool value, uint32_t size) {
    if (value) {
        SCCUAPP << "set OFF";
        return true;

    }
    SCCUAPP << "set OFF";

    return false;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setStby(const std::string &name, bool value, uint32_t size) {
    if (value) {
        SCCUAPP << "set Standby";
        return powerStandby(true);
    } else {
        SCCUAPP << "set Operational";
        return powerON(true);
    }

    return false;
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setPol(const std::string &name, int32_t value, uint32_t size) {
    SCCUAPP << "set polarity:" << value;

    return setPolarity(value,true);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setRampH(const std::string &name, double value, uint32_t size) {
    int err = -1;

    const double *asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampDownRate");
    if (value > 0 && *asdown > 0) {
        SCCUAPP << "set ramp up:" << value;

        err = powersupply_drv->setCurrentRampSpeed(value, *asdown);
    }
    return (err != chaos::ErrorCode::EC_NO_ERROR);
}

bool ::driver::powersupply::SCPowerSupplyControlUnit::setRampL(const std::string &name, double value, uint32_t size) {
    int err = -1;
    const double *asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampUpRate");
    if (value > 0 && *asup > 0) {
        SCCUAPP << "set ramp down:" << value;

        err = powersupply_drv->setCurrentRampSpeed(*asup, value);
    }
    return (err != chaos::ErrorCode::EC_NO_ERROR);
}

/*
 Return the default configuration
 */
void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineActionAndDataset() throw (chaos::CException) {
    //install all command
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdPSDefault), true);
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
    addAttributeToDataSet("bypass",
            "exclude HW changes",
            DataType::TYPE_BOOLEAN,
            DataType::Input);

    /// Dataset per analog dataset element properties
    addAttributeToDataSet("unit",
            "current/voltage units",
            DataType::TYPE_STRING,
            DataType::Input,
            256);

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
            DataType::TYPE_INT32,
            DataType::Input);

    addAttributeToDataSet("setTimeout",
            "timeout for a SET operation(us)",
            DataType::TYPE_INT32,
            DataType::Input);
    addAttributeToDataSet("getTimeout",
            "timeout for a SET operation(us)",
            DataType::TYPE_INT32,
            DataType::Input);

    addBinaryAttributeAsSubtypeToDataSet("conversionFactor", "Array of double Coefficents used to convert value from the CU actual units and the driver elementary units", chaos::DataType::SUB_TYPE_DOUBLE, 1, chaos::DataType::Input);

    addAttributeToDataSet("resolution",
            "Double Minimum meaningful variation of the set respect to the last accepted one",
            DataType::TYPE_DOUBLE,
            DataType::Input);

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

    addHandlerOnInputAttributeName< ::driver::powersupply::SCPowerSupplyControlUnit, long long >(this,
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

}

void ::driver::powersupply::SCPowerSupplyControlUnit::unitDefineCustomAttribute() {

}

// Abstract method for the initialization of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitInit() throw (CException) {
     metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,"Initializing");

    int err = 0;
    int state_id;
    std::string max_range;
    std::string min_range;
    std::string state_str;
    RangeValueInfo attr_info;

    const double *asup = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampUpRate");
    const double *asdown = getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "rampDownRate");
    const bool* s_bypass = getAttributeCache()->getROPtr<bool>(DOMAIN_INPUT, "bypass");
    chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = *s_bypass && (getAccessoInstanceByIndex(1)) ? getAccessoInstanceByIndex(1) : getAccessoInstanceByIndex(0);

    //chaos::cu::driver_manager::driver::DriverAccessor *power_supply_accessor = getAccessoInstanceByIndex(0);

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


    // retrive the attribute description from the device database
    /*
     * current_sp_attr_info.reset();
     */
    if (*asup <= 0) {
        //throw chaos::CException(-4, "No slop up speed set", __FUNCTION__);
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"No slop up speed set" );

    }
    if (*asdown <= 0) {
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"No slop down speed set" );
    }


    if (powersupply_drv->getState(&state_id, state_str, 30000) != 0) {
        throw chaos::CException(-6, "Error getting the state of the powersupply, possibily off", __FUNCTION__);
    }
    //notify change on status_id cached attribute
    getAttributeCache()->setOutputDomainAsChanged();

    if (powersupply_drv->getHWVersion(device_hw, 1000) == 0) {
        SCCUAPP << "hardware found: \"" << device_hw << "\"";
    }


    SCCUAPP << "set default slope value up:" << *asup << " down:" << *asdown;
    err = powersupply_drv->setCurrentRampSpeed(*asup, *asdown);
    if ((err != chaos::ErrorCode::EC_NO_ERROR) && (err != chaos::ErrorCode::EC_NODE_OPERATION_NOT_SUPPORTED)) {
          metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error setting Ramp Speep %1% %2%") % *asup %*asdown) );

     //   throw chaos::CException(-7, "Error setting slope ", __FUNCTION__);
        //TODO: check the  boost::bad_format_string: format-string is ill-formed
        //throw chaos::CException(2, boost::str( boost::format("Error %1 setting the slope in state %2%[%3%]") % err % state_str % state_id), std::string(__FUNCTION__));
    }
         metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,"Init Done");

}

// Abstract method for the start of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitStart() throw (CException) {

}

// Abstract method for the stop of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitStop() throw (CException) {

}

// Abstract method for the deinit of the control unit

void ::driver::powersupply::SCPowerSupplyControlUnit::unitDeinit() throw (CException) {
    SCCUAPP << "deinitializing ";
    powersupply_drv->deinit();

}

//! restore the control unit to snapshot
#define RESTORE_LAPP SCCUAPP << "[RESTORE-" <<getCUID() << "] "
#define RESTORE_LERR SCCUERR << "[RESTORE-" <<getCUID() << "] "

bool ::driver::powersupply::SCPowerSupplyControlUnit::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache * const snapshot_cache) throw (chaos::CException) {
    RESTORE_LAPP << "Check if restore cache has the needed data";
    //check if in the restore cache we have all information we need
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

    bool islocal = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "local")->getValuePtr<bool>();
    if (islocal) {
        RESTORE_LAPP << "Powersupply is in local so no restore can be applied, return OK";
        return true;
    }

    RESTORE_LAPP << "Start the restore of the powersupply";
    uint64_t start_restore_time = chaos::common::utility::TimingUtil::getTimeStamp();
    bool cmd_result = true;
    //get actual state
    double *now_current_sp = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "current");
    bool *now_stby = getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "stby");
    int32_t *now_polarity = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "polarity");
    uint64_t *alarm=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "alarms");
    if(*alarm!=0){
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,CHAOS_FORMAT("There are active alarms %1% during restore, try to rese",%*alarm ));
        RESTORE_LAPP << " resetting alarms";

        if(!setAlarms(1)){
              RESTORE_LERR << " 2 error resetting alarms";
           return false;
        }
    }
    //chec the restore polarity
    int32_t restore_polarity = *snapshot_cache->getAttributeValue(DOMAIN_INPUT, "polarity")->getValuePtr<int32_t>();
    double restore_current_sp = *snapshot_cache->getAttributeValue(DOMAIN_INPUT, "current")->getValuePtr<double>();
    bool restore_stby = *snapshot_cache->getAttributeValue(DOMAIN_INPUT, "stby")->getValuePtr<bool>();
    int is_bipolar = powersupply_drv->getFeatures()& ::common::powersupply::POWER_SUPPLY_FEAT_BIPOLAR;
    RESTORE_LAPP << "current SP:" << *now_current_sp << "==> " << restore_current_sp;
    RESTORE_LAPP << "current STBY:" << *now_stby << "==>" << restore_stby;

    RESTORE_LAPP << "current POLARITY:" << *now_polarity << "==>" << restore_polarity;

    // restoring mode
    if (restore_stby != *now_stby) {
        if (*now_stby == false) {
            // we are in on we should go in stby
            RESTORE_LAPP << "1 set current to 0";
            if (!setCurrent(0)) {
                RESTORE_LERR << " 2 error setting current to 0";
                return false;
            }
            RESTORE_LAPP << "3 going to standby";

            if (!powerStandby()) {
                SCCUERR << " 4 error going in stby";
                return false;
            }
            if ((is_bipolar == 0) && (*now_polarity != restore_polarity)) {
                RESTORE_LAPP << "5 changing polarity:" << restore_polarity;

                if (!setPolarity(restore_polarity)) {
                    RESTORE_LERR << "6 error restoring polariy to " << restore_polarity;
                    return false;
                }
            }
        } else {
            //we are in off we should go on

            // restore polarity
            if ((is_bipolar == 0) && (*now_polarity != restore_polarity)) {
                RESTORE_LAPP << "7 changing polarity:" << restore_polarity;

                if (!setPolarity(restore_polarity)) {
                    RESTORE_LERR << "8 error restoring polariy to " << restore_polarity;
                    return false;
                }
                RESTORE_LAPP << "9 going power on";
            }
              
            if (powerON() == false) {
                    RESTORE_LERR << " 10 error restoring poweron";
                    return false;
                
            }
            RESTORE_LAPP << "11 restoring current:" << restore_current_sp;

           if (!setCurrent(restore_current_sp)) {
            RESTORE_LERR << "12 error restoring current to " << restore_current_sp;
            return false;
            }
            uint64_t restore_duration_in_ms = chaos::common::utility::TimingUtil::getTimeStamp() - start_restore_time;
            RESTORE_LAPP << "[metric] Restore successfully achieved in " << restore_duration_in_ms << " milliseconds";
            return true;
        }    

    }
    //handle bipolar

    ///
    if ((*now_polarity != restore_polarity)&&(is_bipolar == 0)) {
        //we need to change the polarity
        RESTORE_LAPP << "Change the polarity from:" << *now_polarity << " to:" << restore_polarity;

        //put in standby
        RESTORE_LAPP << "Put powersupply at setpoint 0";
        if (setCurrent(0.0)) {
            RESTORE_LAPP << "going to standby";

            if (powerStandby()) {
                RESTORE_LAPP << "Powersupply is gone in standby";
            } else {
                SCCUERR << "Power supply is not gone in standby";
                return false;
            }
        } else {
            RESTORE_LERR << "Power supply is not gone to 0 ampere";
            return false;
        }
        //set the polarity
        RESTORE_LAPP << "Apply new polarity:" << restore_polarity;
        if (!setPolarity(restore_polarity)) {
            RESTORE_LERR << "Power supply is not gone to restore polarity" << restore_polarity;
            return false;
        }
        if (restore_stby == false) {
            RESTORE_LAPP << "going power on";

            if (powerON() == false) {
                RESTORE_LERR << " error restoring poweron";
                return false;
            }
        }
    }
    RESTORE_LAPP << "setting current:" << restore_current_sp;

    if (!setCurrent(restore_current_sp)) {
        SCCUERR << "## error restoring current:" << restore_current_sp;
        return false;
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

bool ::driver::powersupply::SCPowerSupplyControlUnit::setAlarms(long long alarms,
        bool sync) {
    uint64_t cmd_id;
    bool result = true;
    std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
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
            SubmissionRuleType::SUBMIT_NORMAL);
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
                SCCUAPP << cmd_id << " -> FAULT";
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
