/*
 *	RTCORPowerSupply.h
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
#ifndef __PowerSupply__RTCORPowerSupply__
#define __PowerSupply__RTCORPowerSupply__

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>
#include <driver/data-import/models/dante/DanteDriver.h>
namespace driver {
	namespace powersupply {
		class RTCORPowerSupply : public chaos::cu::control_manager::RTAbstractControlUnit {

			PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTCORPowerSupply)

			// init paramter
			  std::string device_hw;
			
    		::driver::data_import::DanteDriver driver;
			chaos::common::data::CDataWrapper in,out,pin,pout;		


		protected:
			int acquireOut();
			int acquireIn();
			void setFlags();
			/*
			 Define the Control Unit Dataset and Actions
			 */
			void unitDefineActionAndDataset();

			void unitDefineCustomAttribute();
			
			/*(Optional)
			 Initialize the Control Unit and all driver, with received param from MetadataServer
			 */
			void unitInit() ;
			/*(Optional)
			 Execute the work, this is called with a determinated delay, it must be as fast as possible
			 */
			void unitStart() ;
			/*(Optional)
			 The Control Unit will be stopped
			 */
			void unitStop() ;
			/*(Optional)
			 The Control Unit will be deinitialized and disposed
			 */
			void unitDeinit() ;

            //!restore method for powersupply

		
			uint64_t alarms[2];
		public:
			/*
			 Construct a new CU with an identifier
			 */
			RTCORPowerSupply(const std::string& _control_unit_id,
									 const std::string& _control_unit_param,
									 const ControlUnitDriverList& _control_unit_drivers);
			
			/*
			 Base destructor
			 */
			~RTCORPowerSupply();
			 
			
    		void unitRun() ;
  			 chaos::common::data::CDWUniquePtr getProperty(chaos::common::data::CDWUniquePtr);
  			 chaos::common::data::CDWUniquePtr setProperty(chaos::common::data::CDWUniquePtr);


		};


	}
}

#endif /* defined(__PowerSupply__RTCORPowerSupply__) */