/*
 *	main.cpp
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

#include "driver/powersupply/models/GenericPowerSupplyDD.h"
#include "SCPowerSupplyControlUnit.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>
#include <chaos/common/exception/CException.h>

#include <iostream>
#include <string>

/*! \page page_example_cue ChaosCUToolkit Example
 *  \section page_example_cue_sec An basic usage for the ChaosCUToolkit package
 *
 *  \subsection page_example_cue_sec_sub1 Toolkit usage
 *  ChaosCUToolkit has private constructor so it can be used only using singleton pattern,
 *  the only way to get unique isntance is; ChaosCUToolkit::getInstance(). So the first call of
 *  getInstance method will provide the CUToolkit and Common layer initial setup.
 *  \snippet example/ControlUnitTest/ControlUnitExample.cpp Custom Option
 */
using namespace std;
using namespace chaos;
using namespace chaos::cu;
using namespace chaos::driver::powersupply;


namespace common_plugin = chaos::common::plugin;
namespace common_utility = chaos::common::utility;
namespace cu_driver_manager = chaos::cu::driver_manager;



#define OPT_DEVICE_ID			"ps_id"
#define OPT_DRIVER_PARAMETERS   "driver"

#define OPT_SC_DEVICE_ID			"sc_ps_id"
#define OPT_SC_DRIVERS_PARAMETERS	"sc_driver"
// initialisation format for simulator <serial port>,<slaveid>,<write_latency_min:write_latency_max>,<read_latency_min:read_latency_min>,<maxcurr:max voltage>

static const boost::regex power_supply_simulator_init_match("([\\w\\/]+),(\\d+),(\\d+):(\\d+),(\\d+):(\\d+),(\\d+):(\\d+)");

int main (int argc, char* argv[] )
{
    string tmp_device_id,driver_params;
	string tmp_definition_param;
	string tmp_address;
	
	vector< string > sc_device_ids;
	vector< string > sc_device_param;
    try {
		//! [Custom Option]
		ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_DRIVER_PARAMETERS, po::value<string>(&driver_params), "Specify the driver params <DRIVERNAME:'driver specific params' ie:OcemE642X:/dev/ttyr00,10> or SimPSupply:<dev>,<id>,<write_latency_min:write_latency_max>,<read_latency_min:read_latency_max>,<maxcurr:max voltage>");
		ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SC_DEVICE_ID, po::value< vector< string > >(&sc_device_ids)->multitoken(), "Specify the id's of the slow power supply slow contorl cu");
		ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SC_DRIVERS_PARAMETERS, po::value< vector< string > >(&sc_device_param)->multitoken(), "Specify the id's of the slow power supply slow contorl cu");
		
		
		//! [Custom Option]
		
		//! [CUTOOLKIT Init]
		ChaosCUToolkit::getInstance()->init(argc, argv);
		
		//! [CUTOOLKIT Init]
		
		//! [Driver Registration]
		MATERIALIZE_INSTANCE_AND_INSPECTOR_WITH_NS(chaos::driver::powersupply, GenericPowerSupplyDD)
		cu_driver_manager::DriverManager::getInstance()->registerDriver(GenericPowerSupplyDDInstancer, GenericPowerSupplyDDInspector);
		
		chaos::cu::driver_manager::driver::DrvRequestInfo drv1 = {"GenericPowerSupplyDD", "1.0.0", driver_params.c_str() };
		chaos::cu::control_manager::AbstractControlUnit::ControlUnitDriverList driver_list; driver_list.push_back(drv1);
		ChaosCUToolkit::getInstance()->registerControlUnit< ::driver::powersupply::SCPowerSupplyControlUnit >();
		//! [Driver Registration]
		
		//! [Adding the CustomControlUnit
		bool sc_cu_ok = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SC_DEVICE_ID) &&
						ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SC_DRIVERS_PARAMETERS);
		
		
		if(sc_cu_ok) {
		  cout<< "selected SC CU"<<endl;
			//install all slowcontrol cu for deviceids
			if(sc_device_ids.size() == sc_device_param.size()) {
				for (int idx = 0; idx <
					 sc_device_ids.size();
					 idx++) {
					ChaosCUToolkit::getInstance()->addControlUnit(new ::driver::powersupply::SCPowerSupplyControlUnit(sc_device_ids[idx], sc_device_param[idx], driver_list));
				}
			} else {
				throw CException(3, "sc device ids and sc parameter need to have same number of values", __FUNCTION__);
			}
		}
		//! [Adding the CustomControlUnit]
		
		//! [Starting the Framework]
		if(sc_cu_ok) {
			ChaosCUToolkit::getInstance()->start();
		} else {
		  cout<<"## you must select a CU type"<<endl;
		  return -1;
		}
		//! [Starting the Framework]
	} catch (CException& e) {
		cerr<<"Exception:"<<endl;
		std::cerr<< "domain	:"<<e.errorDomain << std::endl;
		std::cerr<< "cause	:"<<e.errorMessage << std::endl;
	} catch (program_options::error &e){
		cerr << "Unable to parse command line: " << e.what() << endl;
	} catch (...){
		cerr << "unexpected exception caught.. " << endl;
	}
	
	return 0;
}
