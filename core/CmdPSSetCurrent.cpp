/*
 *	CmdPSSetCurrent.cpp
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


#include "CmdPSSetCurrent.h"

#include <cmath>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

#define SCLAPP_ INFO_LOG(CmdPSSetCurrent) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdPSSetCurrent) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdPSSetCurrent) << "[" << getDeviceID() << "] "


namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

using namespace chaos::cu::control_manager;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::powersupply::,CmdPSSetCurrent,CMD_PS_SET_CURRENT_ALIAS,
                                                          "Set current (A) to the given value",
                                                          "72882f3e-35da-11e5-985f-334fcd6dff22")
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_PS_SET_CURRENT, "current in A",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()


void own::CmdPSSetCurrent::setHandler(c_data::CDataWrapper *data) {
    chaos::common::data::RangeValueInfo current_sp_attr_info;
    chaos::common::data::RangeValueInfo attributeInfo;
	AbstractPowerSupplyCommand::setHandler(data);
        AbstractPowerSupplyCommand::acquireHandler();

	double max_current=0,min_current=0;
	int err = 0;
	int state;
	std::string state_str;
	float current = 0.f;
	float slope_speed = 0.f;
        chaos::common::data::RangeValueInfo attr_info;
	getDeviceDatabase()->getAttributeRangeValueInfo("current", attr_info);
        setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);
        setStateVariableSeverity(StateVariableTypeAlarmCU,"current_out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);

  // REQUIRE MIN MAX SET IN THE MDS
        if (attr_info.maxRange.size()) {
            max_current = atof(attr_info.maxRange.c_str());
          SCLDBG_ << "max_current max=" << max_current;

        } else {
                 SCLERR_ << "not defined maximum 'current voltage' attribute, quitting command";
                 metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"not defined maximum 'current voltage' attribute, quitting command" );
                 setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

                 BC_FAULT_RUNNING_PROPERTY;
                 return;
        }

        // REQUIRE MIN MAX SET IN THE MDS
        if (attr_info.minRange.size()) {
              min_current = atof(attr_info.minRange.c_str());

              SCLDBG_ << "min_current min=" << min_current;
        } else {
               SCLERR_ << "not defined minimum 'current voltage' attribute, quitting command";
               setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                 metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"not defined minimum 'current voltage' attribute, quitting command" );

               BC_FAULT_RUNNING_PROPERTY;
               return;

        }
        SCLDBG_<<"minimum working value:"<<*p_minimumWorkingValue;
        SCLDBG_<<"maximu, working value:"<<*p_maximumWorkingValue;
        
        //min_current=std::max(*p_minimumWorkingValue,min_current);
        //max_current=std::min(max_current,*p_maximumWorkingValue);
        
	
	current = 0;

	//set comamnd timeout for this instance
	SCLDBG_ << "Checking for timeout";
	
	if(!data ||
	   !data->hasKey(CMD_PS_SET_CURRENT)) {
		SCLERR_ << "Set current parameter not present";
                setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

		BC_FAULT_RUNNING_PROPERTY;
		return;
	}
	if(!data->isDoubleValue(CMD_PS_SET_CURRENT)) {
		SCLERR_ << "Set current parameter is not a Double data type";
                setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

		BC_FAULT_RUNNING_PROPERTY;
		return;
	}
    
    current = static_cast<float>(data->getDoubleValue(CMD_PS_SET_CURRENT));
    SCLAPP_<<"set current:"<<current;
    if(isnan(current)==true){
        SCLERR_ << "Set current parameter is not a valid double number (nan?)";
        setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

        BC_FAULT_RUNNING_PROPERTY;
        return;
    }
    if(current>max_current || current<min_current){
          std::stringstream ss;
        ss<<"current:"<<current<<" > "<<max_current;
                setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                 metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("current %1% outside  the maximum/minimum 'current' \"max_current\":%2% \"min_current\":%3%" , % current % max_current % min_current));

		BC_FAULT_RUNNING_PROPERTY;
		return;
    }
    double delta=abs(current-*o_current);
    SCLDBG_ << "delta current = " << delta;

    if(delta<*p_resolution){
        SCLDBG_ << "operation inibited because of resolution:" << *p_resolution;
         metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,CHAOS_FORMAT("operation inibited because of resolution %1% , delta current %2%",%*p_resolution %delta ));
        *i_current = current;
        getAttributeCache()->setInputDomainAsChanged();

    		BC_END_RUNNING_PROPERTY;
		return;
    }

        SCLDBG_ << "compute timeout for set current = " << current;
	if(*o_current > current) {
		SCLDBG_ << "The new current is lower then actual = " << *o_current << "[new "<<current<<"]";
		slope_speed  = *i_asdown;
	}else {
		SCLDBG_ << "The new current is higher then actual = " << *o_current << "[new "<<current<<"]";;
		slope_speed  = *i_asup;
	}
        uint32_t computed_timeout = uint64_t(((delta / slope_speed) * 1000)) + DEFAULT_RAMP_TIME_OFFSET_MS;

        computed_timeout=std::max(computed_timeout,*p_setTimeout);
        
	//compute the delta for check if w
	SCLDBG_ << "Slope speed is = " << slope_speed;
	SCLDBG_ << "Calculated timout is = " << computed_timeout;
	setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, computed_timeout);
	//set current set poi into the output channel

	SCLDBG_ << "Set current to value " << current;
	if((err = powersupply_drv->setCurrentSP(current)) != 0) {
            metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,boost::str( boost::format("Error setting current %1%") % current) );

            setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

            BC_FAULT_RUNNING_PROPERTY;
            return;
	}
       
	//assign new current setpoint
	*i_current = current;
	setWorkState(true);
        getAttributeCache()->setInputDomainAsChanged();
      //  pushInputDataset();
        setStateVariableSeverity(StateVariableTypeAlarmCU,"current_value_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelClear);
        if(*o_stby){
         // we are in standby only the SP is set
             SCLDBG_ << "we are in standby we cannot start ramp SP: "<<*i_current;

         BC_END_RUNNING_PROPERTY;
         return;
        } 
        if((err = powersupply_drv->startCurrentRamp()) != 0) {
              SCLERR_<<"## error setting current ramp "<<current;
              setStateVariableSeverity(StateVariableTypeAlarmCU,"current_invalid_set", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

              BC_FAULT_RUNNING_PROPERTY;
              return;
          }
        
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,boost::str( boost::format("performing command set current :%1% timeout %2%") % current % computed_timeout) );

	BC_NORMAL_RUNNING_PROPERTY;

}

void own::CmdPSSetCurrent::acquireHandler() {
	//acquire the current readout
        AbstractPowerSupplyCommand::acquireHandler();

	//force output dataset as changed
	getAttributeCache()->setOutputDomainAsChanged();
}

void own::CmdPSSetCurrent::ccHandler() {
	//check if we are int the delta of the setpoit to end the command
	double delta_current_reached = std::abs(*o_current - *i_current);
	SCLDBG_ << "Readout: "<< *o_current <<" SetPoint: "<< *i_current<<" Delta to reach: " << delta_current_reached;
	if(delta_current_reached <= *p_resolution || delta_current_reached<*p_warningThreshold) {
		uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
		//the command is endedn because we have reached the affinitut delta set
		SCLDBG_ << "[metric ]Set point reached with - delta: "<< delta_current_reached <<" sp: "<< *i_current <<" affinity check " << *p_warningThreshold << " ampere in " << elapsed_msec << " milliseconds";
		BC_END_RUNNING_PROPERTY;
        }
        
	if(*o_alarms) {
		SCLERR_ << "We got alarms on powersupply so we end the command";
		BC_END_RUNNING_PROPERTY;
	}
}

bool own::CmdPSSetCurrent::timeoutHandler() {
	double delta_current_reached = std::abs(*i_current - *o_current);
	uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
	//move the state machine on fault
         metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,CHAOS_FORMAT("timeout, delta current remaining %1%",%delta_current_reached));

	if(delta_current_reached <= *p_resolution || delta_current_reached<*p_warningThreshold) {
		uint64_t elapsed_msec = chaos::common::utility::TimingUtil::getTimeStamp() - getSetTime();
		//the command is endedn because we have reached the affinitut delta set
		SCLDBG_ << "[metric ]Set point reached with - delta: "<< delta_current_reached <<" sp: "<< *i_current <<" affinity check " << *p_warningThreshold << " ampere in " << elapsed_msec << " milliseconds";
        } else {
		SCLERR_ << "[metric] Setpoint not reached on timeout with readout current " << *o_current << " in " << elapsed_msec << " milliseconds";
                setStateVariableSeverity(StateVariableTypeAlarmCU,"value_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

		
	}
        setWorkState(false);

        BC_END_RUNNING_PROPERTY
	return false;
}
