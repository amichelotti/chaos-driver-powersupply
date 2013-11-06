/*
 *	PowerSupplyCostants.h
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


#ifndef PowerSupply_PowerSupplyCostants_h
#define PowerSupply_PowerSupplyCostants_h

namespace driver {
	namespace powersupply {
		
		//! The alias of the default command of the type
		const char * const CMD_PS_DEFAULT_ALIAS = "cmd_ps_dafault";
		
		//! The alias of the "mode" command of the type
		const char * const CMD_PS_MODE_ALIAS = "cmd_ps_mode";

		//! The alias of the parameter of the type
		const char * const CMD_PS_MODE_TYPE = "mode_type";
	}
}
#endif
