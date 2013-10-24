/*
 *	SiemensS7TcpDriver.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
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
#ifndef __driver_PowerSupply_h__
#define __driver_PowerSupply_h__


// include your class/functions headers here

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

//this need to be out the nasmespace
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(PowerSupplyDriver)

namespace driver {
	namespace PowerSupply {
		namespace cu_driver = chaos::cu::driver_manager::driver;
		
		/*
		 driver definition
		 */
		class PowerSupplyDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
			
			void driverInit(const char *initParameter) throw(chaos::CException);
			void driverDeinit() throw(chaos::CException);
			
		public:
			PowerSupplyDriver();
			~PowerSupplyDriver();
			//! Execute a command
			cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);
		};
	}
}

#endif /* defined(__ControlUnitTest__SiemensS7TcpDriver__) */
