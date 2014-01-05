/*
 *	CmdSetPolarity.cpp
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

#include "CmdSetPolarity.h"


#include <boost/format.hpp>

#define SCLAPP_ LAPP_ << "[CmdSetPolarity -" << LOG_TAIL
#define SCLDBG_ LDBG_ << "[CmdSetPolarity -" << LOG_TAIL
#define SCLERR_ LERR_ << "[CmdSetPolarity -" << LOG_TAIL


namespace own =  driver::powersupply;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

    // return the implemented handler
uint8_t own::CmdSetPolarity::implementedHandler() {
    return	chaos_batch::HandlerType::HT_Set;
}

void own::CmdSetPolarity::setHandler(c_data::CDataWrapper *data) {
	AbstractPowerSupplyCommand::setHandler(data);
	int err = 0;
    
    
    if(!data || !data->hasKey(CMD_PS_SET_POLARITY_VALUE) ) {
        throw chaos::CException(1, "Type of polarity not passed", std::string(__FUNCTION__));
    }
	
    int32_t polarity = data->getInt32Value(CMD_PS_SET_POLARITY_VALUE);
    SCLAPP_ << "Set polarity called with value " << polarity;
    
	switch (*o_status_id) {
		case common::powersupply::POWER_SUPPLY_STATE_ALARM:
		case common::powersupply::POWER_SUPPLY_STATE_ERROR:
		case common::powersupply::POWER_SUPPLY_STATE_UKN:
        case common::powersupply::POWER_SUPPLY_STATE_OPEN:
		case common::powersupply::POWER_SUPPLY_STATE_ON:
                //i need to be in operational to exec
			throw chaos::CException(1, boost::str( boost::format("Bad state for set polarity comamnd %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
			break;
			

		case common::powersupply::POWER_SUPPLY_STATE_STANDBY:
			SCLDBG_ << "We can start the set polarity command";
			break;
			
		default:
			throw chaos::CException(2, boost::str( boost::format("Unrecognized state %1%[%2%]") % o_status % *o_status_id), std::string(__FUNCTION__));
	}
	
        //set comamnd timeout for this instance
	SCLDBG_ << "Checking for timout";
	if(*i_command_timeout) {
		SCLDBG_ << "Timeout will be set to ms -> " << *i_command_timeout;
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, *i_command_timeout);
	}
	
	if((err = powersupply_drv->setPolarity(polarity)) != 0) {
          throw chaos::CException(1, boost::str( boost::format("Set current parameter not present") % o_status % *o_status_id), std::string(__FUNCTION__));
	}
    
    BC_END_RUNNIG_PROPERTY
}