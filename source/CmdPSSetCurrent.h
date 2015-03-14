/*
 *	CmdPSSetCurrent.h
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

#ifndef __PowerSupply__CmdSetCurrent__
#define __PowerSupply__CmdSetCurrent__


#include "AbstractPowerSupplyCommand.h"

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

namespace driver {
	namespace powersupply {
		
		//! Command for change the mode of the powersupply
		class CmdPSSetCurrent : public AbstractPowerSupplyCommand {
			float asup;
			float asdown;
			double	*o_current_sp;
			const double	*o_current;
			const double	*i_slope_up;
			const double	*i_slope_down;
			const uint32_t	*i_command_timeout;
			const uint32_t	*i_delta_setpoint;
			const uint32_t	*i_setpoint_affinity;
		protected:
			//implemented handler
			uint8_t implementedHandler();
			
			// Set handler
			void setHandler(c_data::CDataWrapper *data);
			
			//Correlation and commit phase
			void ccHandler();
			
			//manage the timeout
			bool timeoutHandler();
		};
	}
}


#endif /* defined(__PowerSupply__CmdSetCurrent__) */