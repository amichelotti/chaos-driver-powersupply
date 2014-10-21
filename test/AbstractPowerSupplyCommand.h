/*
 *	AbstractPowerSupplyCommand.h
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

#ifndef __PowerSupply__AbstractPowerSupplyCommand__
#define __PowerSupply__AbstractPowerSupplyCommand__

#include "PowerSupplyCostants.h"

#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

namespace driver {
	namespace powersupply {
		
		class AbstractPowerSupplyCommand : public ccc_slow_command::SlowCommand {
		protected:
			//output variable
			char		*o_cmd_last_error;
			uint64_t	*o_dev_state;
			char		*o_status;
			int32_t		*o_status_id;
			uint64_t	*o_alarms;
			int32_t		*o_polarity;
			double		*o_voltage;
			double		*o_current_sp;
			double		*o_current;
			int32_t		*o_on;
			int32_t		*o_stby;
			int32_t		*o_alarm;
			//input variable
			const double	*i_slope_up;
			const double	*i_slope_down;
			const uint32_t	*i_command_timeout;
			const uint32_t	*i_driver_timeout;
			const uint32_t	*i_delta_setpoint;
			const uint32_t	*i_setpoint_affinity;
			//reference of the chaos bastraction ofpowersupply driver
			chaos::driver::powersupply::ChaosPowerSupplyInterface *powersupply_drv;
			
			//implemented handler
			uint8_t implementedHandler();
			
			void ccHandler();
			
			// set the data fr the command
			void setHandler(c_data::CDataWrapper *data);
			
			bool checkState(common::powersupply::PowerSupplyStates state_to_check);
			void getState(int& current_state, std::string& current_state_str);
			
			void writeErrorMessage(string error_message);
			void writeErrorMessage(const char * error_message);
			
			void setWorkState(bool working);
		};
	}
}

#endif /* defined(__PowerSupply__AbstractPowerSupplyCommand__) */
