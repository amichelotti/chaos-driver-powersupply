/*
 *	CmdPSDefault.cpp
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

#include <string.h>
#include "CmdPSDefault.h"
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

#define LOG_HEAD_CmdPSDefault LOG_TAIL(CmdPSDefault)

#define CMDCU_ INFO_LOG(CmdPSDefault) << "[" << getDeviceID() << "] "
#define CMDCUDBG_ DBG_LOG(CmdPSDefault) << "[" << getDeviceID() << "] "
#define CMDCUERR_ ERR_LOG(CmdPSDefault) << "[" << getDeviceID() << "] "

using namespace driver::powersupply;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::control_manager;


BATCH_COMMAND_OPEN_DESCRIPTION(driver::powersupply::,CmdPSDefault,
                                                          "Default method",
                                                          "4403148a-35df-11e5-b1f7-7f8214ad6212")
BATCH_COMMAND_CLOSE_DESCRIPTION()
CmdPSDefault::CmdPSDefault() {
	powersupply_drv = NULL;
        start_out_of_set_time=0;
}

CmdPSDefault::~CmdPSDefault() {
	
}

    // return the implemented handler

    // Start the command execution
void CmdPSDefault::setHandler(c_data::CDataWrapper *data) {
	
	AbstractPowerSupplyCommand::setHandler(data);
        setWorkState(false);

	BC_NORMAL_RUNNING_PROPERTY
  }

    // Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void CmdPSDefault::acquireHandler() {
	AbstractPowerSupplyCommand::acquireHandler();
	//force output dataset as changed
	
}
void CmdPSDefault::ccHandler() {
      setWorkState(false);

    /////  CHECKS during operational mode
     if(*i_stby!=*o_stby){
        setStateVariableSeverity(StateVariableTypeWarning,"stby_out_of_set",chaos::common::alarm::MultiSeverityAlarmLevelWarning);
    } else {
        setStateVariableSeverity(StateVariableTypeWarning,"stby_out_of_set",chaos::common::alarm::MultiSeverityAlarmLevelClear);
        setStateVariableSeverity(StateVariableTypeWarning,"stby_value_not_reached",chaos::common::alarm::MultiSeverityAlarmLevelClear);

    }
    if(*o_alarms){
       setStateVariableSeverity(StateVariableTypeAlarm,"interlock", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
       return;
    }
    setStateVariableSeverity(StateVariableTypeAlarm,"interlock", chaos::common::alarm::MultiSeverityAlarmLevelClear);
   
    
    if(*o_stby == 0){
        if(*p_warningThreshold>0){
            //enable out of set warning

            double err=fabs(*o_current-*i_current);
            if(err>*p_warningThreshold){
                if(start_out_of_set_time==0){
                    start_out_of_set_time= chaos::common::utility::TimingUtil::getTimeStamp() ;
                }
                uint64_t tdiff=chaos::common::utility::TimingUtil::getTimeStamp() -start_out_of_set_time;
                if(tdiff>*p_warningThresholdTimeout){
                   setStateVariableSeverity(StateVariableTypeWarning,"current_out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                   CMDCUDBG_<<"current out of set detected diff:"<< err << " after "<<tdiff<< " ms";

                }

            } else {
                start_out_of_set_time=0;
                 setStateVariableSeverity(StateVariableTypeWarning,"current_out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);
                 setStateVariableSeverity(StateVariableTypeWarning,"current_value_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelClear);

            }
        }
    
        if(*o_stby && (*i_pol!=*o_pol)){
             setStateVariableSeverity(StateVariableTypeWarning,"polarity_out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

        } else {
             setStateVariableSeverity(StateVariableTypeWarning,"polarity_out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);
             setStateVariableSeverity(StateVariableTypeWarning,"polarity_value_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelClear);


        }
    }
    getAttributeCache()->setOutputDomainAsChanged();
}
