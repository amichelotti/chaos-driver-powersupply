/*
 *	PowerSupplyControlUnit.h
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

#ifndef __driver_PowerSupplyControlUnit_h__
#define __driver_PowerSupplyControlUnit_h__

#include <chaos/cu_toolkit/ControlManager/RTAbstractControlUnit.h>
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

namespace driver {
	namespace powersupply {
		
		class PowerSupplyControlUnit : public chaos::cu::RTAbstractControlUnit {
			// init paramter
			string device_id;
			string params;
            chaos::driver::powersupply::ChaosPowerSupplyInterface *powersupply_drv;
            
            string device_hw;
            float current_sp;
		protected:
			//define dataset
			void unitDefineActionAndDataset()throw(chaos::CException);
			
			//defin control unit driver
			void unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver);
			
			// init contorl unit
			void unitInit() throw(chaos::CException);
			
			//start contor unit
			void unitStart() throw(chaos::CException);
			
			//intervalled scheduled method
			void unitRun() throw(chaos::CException);
			
			//stop contor unit
			void unitStop() throw(chaos::CException);
			
			//deinit
			void unitDeinit() throw(chaos::CException);
            
            void setCurrent(const std::string& deviceID, const double& current);
            void setAlimStatus(const std::string& deviceID, const int32_t& current);
            void setPolarity(const std::string& deviceID, const int32_t& current);
            void clearAlarms(const std::string& deviceID, const int64_t& current);

		public:
			/*
			 Construct a new CU with an identifier
			 */
			PowerSupplyControlUnit(string _device_id,string params);
			
			/*
			 Base destructor
			 */
			~PowerSupplyControlUnit();
		};
	}
}

#endif /* defined(__ControlUnitTest__S7ControlUnit__) */
