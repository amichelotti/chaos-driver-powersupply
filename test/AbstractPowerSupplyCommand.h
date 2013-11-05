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

#include "PowerSupplyStateMachine.h"

#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

namespace driver {
	namespace powersupply {
		
		class AbstractPowerSupplyCommand : public ccc_slow_command::SlowCommand {
		protected:
			//output variable
			char		**o_ps_state;
			uint64_t	*o_dev_state;
			char		**o_status;
			uint64_t	*o_alarms;
			int32_t		*o_polarity;
			double		*o_voltage;
			double		*o_current_sp;
			double		*o_current;
			
			//input variable
			double		*i_slope_up;
			double		*i_slope_down;
			uint32_t	*i_timeout;
			
			//pointer to the power supply state machine
			boost::msm::back::state_machine< powersupply_state_machine_impl > *ps_state_machine_ptr;
			
			//reference of the chaos bastraction ofpowersupply driver
			chaos::driver::powersupply::ChaosPowerSupplyInterface *powersupply_drv;
			
			//implemented handler
			uint8_t implementedHandler();
			
			// set the data fr the command
			void setHandler(c_data::CDataWrapper *data);
			
			const char * getCurrentState();
		};
	}
}

#endif /* defined(__PowerSupply__AbstractPowerSupplyCommand__) */
