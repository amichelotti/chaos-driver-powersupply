/*
 *	SCPowerSupplyControlUnit.h
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
#ifndef __PowerSupply__SCPowerSupplyControlUnit__
#define __PowerSupply__SCPowerSupplyControlUnit__

#include <chaos/cu_toolkit/control_manager/SCAbstractControlUnit.h>
#include <driver/powersupply/core/ChaosPowerSupplyInterface.h>

using namespace chaos;
namespace driver {
	namespace powersupply {
		
		class SCPowerSupplyControlUnit : public chaos::cu::control_manager::SCAbstractControlUnit {
			PUBLISHABLE_CONTROL_UNIT_INTERFACE(SCPowerSupplyControlUnit)

			// init paramter
			  std::string device_hw;
			
			chaos::driver::powersupply::ChaosPowerSupplyInterface *powersupply_drv;

			bool whaitOnCommandID(uint64_t command_id);
		protected:
			/*
			 Define the Control Unit Dataset and Actions
			 */
			void unitDefineActionAndDataset()throw(chaos::CException);

			void unitDefineCustomAttribute();
			
			/*(Optional)
			 Initialize the Control Unit and all driver, with received param from MetadataServer
			 */
			void unitInit() throw(chaos::CException);
			/*(Optional)
			 Execute the work, this is called with a determinated delay, it must be as fast as possible
			 */
			void unitStart() throw(chaos::CException);
			/*(Optional)
			 The Control Unit will be stopped
			 */
			void unitStop() throw(chaos::CException);
			/*(Optional)
			 The Control Unit will be deinitialized and disposed
			 */
			void unitDeinit() throw(chaos::CException);

            //!restore method for powersupply
			bool unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache * const snapshot_cache) throw(CException);

			//-----------utility methdo for the restore operation---------
			bool powerON(bool sync = true);
			bool powerStandby(bool sync = true);
			bool setPolarity(int polarity, bool sync = true);
			bool setCurrent(double current_set_point, bool sync = true);
			bool setRampSpeed(double sup, double sdown, bool sync = true);
                        bool setAlarms(long long value, bool sync = true);

		public:
			/*
			 Construct a new CU with an identifier
			 */
			SCPowerSupplyControlUnit(const std::string& _control_unit_id,
									 const std::string& _control_unit_param,
									 const ControlUnitDriverList& _control_unit_drivers);
			
			/*
			 Base destructor
			 */
			~SCPowerSupplyControlUnit();
			 bool setSP(const std::string &name,double value,uint32_t size);
			 bool setPol(const std::string &name,int value,uint32_t size);
			 bool setRampH(const std::string &name,double value,uint32_t size);
			 bool setRampL(const std::string &name,double value,uint32_t size);
             bool setOff(const std::string &name,bool value,uint32_t size);
             bool setStby(const std::string &name,bool value,uint32_t size);

			 bool setAlarms(const std::string &name,long long value,uint32_t size);



		};


	}
}

#endif /* defined(__PowerSupply__SCPowerSupplyControlUnit__) */
