/*
 *	main.cpp
 *	!CHAOS
 *	Created by Andrea Michelotti
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

#include <driver/powersupply/models/Ocem/OcemDD.h>
#include <driver/powersupply/models/PowerSim/PowerSimDD.h>
#include <driver/powersupply/models/Hazemeyer/ChaosAL250.h>
#include <driver/powersupply/models/ExternalDriver/ChaosPowerSupplyExternalDriver.h>

#include <driver/powersupply/core/SCPowerSupplyControlUnit.h>

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


// initialisation format for simulator <serial port>,<slaveid>,<write_latency_min:write_latency_max>,<read_latency_min:read_latency_min>,<maxcurr:max voltage>

static const std::regex power_supply_simulator_init_match("([\\w\\/]+),(\\d+),(\\d+):(\\d+),(\\d+):(\\d+),(\\d+):(\\d+)");
int main(int argc,const char**argv){
        try{

                chaos::cu::ChaosCUToolkit::getInstance()->init(argc, argv);
                REGISTER_CU(::driver::powersupply::SCPowerSupplyControlUnit); /* file: driver/powersupply/SCPowerSupplyControlUnit.h */
                REGISTER_DRIVER(chaos::driver::powersupply,OcemDD); /* file: driver/powersupply/models/Ocem/OcemDD.h */
                REGISTER_DRIVER(chaos::driver::powersupply,PowerSimDD); 
                REGISTER_DRIVER(chaos::driver::powersupply,C_AL250);
		REGISTER_DRIVER(chaos::driver::powersupply,ChaosPowerSupplyRemoteServerDriver);
		REGISTER_DRIVER(chaos::driver::powersupply,ChaosPowerSupplyRemoteClientDriver);
            
                chaos::cu::ChaosCUToolkit::getInstance()->start();
        } catch (CException& e) {
                std::cerr<<"Exception:"<<std::endl;
                std::cerr<< "domain     :"<<e.errorDomain << std::endl;
                std::cerr<< "cause      :"<<e.errorMessage << std::endl;return -1;
        } catch (program_options::error &e){
                std::cerr << "Unable to parse command line: " << e.what() << std::endl;return -2;
        } catch (...){
                std::cerr << "unexpected exception caught.. " << std::endl;return -3;
        }return 0;
}
