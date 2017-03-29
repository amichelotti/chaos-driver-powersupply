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

#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

namespace driver {
	namespace powersupply {
		
		class AbstractPowerSupplyCommand : public ccc_slow_command::SlowCommand {

		public:
		  AbstractPowerSupplyCommand();
		  ~AbstractPowerSupplyCommand();
		protected:
                    int32_t state;
			bool		*o_stby,*o_local,*i_stby,*i_local,*o_off,*i_off;
            const bool        *c_polFromSet,*c_zeroOnStby,*c_polSwSign,*c_stbyOnPol;
            const bool  *s_bypass;
            const std::string p_unit;
            const int32_t* p_scale;
            const double*p_minimumWorkingValue,*p_maximumWorkingValue,*p_warningThreshold,*p_resolution;
            const uint32_t *p_warningThresholdTimeout,*p_setTimeout,*p_getTimeout,*p_driverTimeout;

            double *o_current,*o_voltage;
            double *i_asup,*i_current,*i_asdown;
            int32_t     *o_pol,*i_pol;
			uint64_t	*o_alarms;
            
			//reference of the chaos bastraction ofpowersupply driver
			chaos::driver::powersupply::ChaosPowerSupplyInterface *powersupply_drv;
			
			//implemented handler
			uint8_t implementedHandler();

			void acquireHandler();
			void ccHandler();
			void endHandler();
			// return 0 on success
			// set the data fr the command
			void setHandler(c_data::CDataWrapper *data);

			void getState(int& current_state, std::string& current_state_str);
			
			void setWorkState(bool working);
		};
	}
}

#endif /* defined(__PowerSupply__AbstractPowerSupplyCommand__) */
