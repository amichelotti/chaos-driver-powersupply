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
		
        #define TROW_ERROR(n,m,d) throw chaos::CException(n, m, d);
		
        #define LOG_TAIL(n) "[" << #n << "] - " << getDeviceID() << " - [" << getUID() << "] - "
        
		//! The alias of the default command of the type
		const char * const CMD_PS_DEFAULT_ALIAS = "default";
		
		//! The alias of the "mode" command of the type
		const char * const CMD_PS_MODE_ALIAS = "mode";
		//! The alias of the parameter of the type {0-to stadby, 1-to operational}
		const char * const CMD_PS_MODE_TYPE = "mode_type";
		
		//! The alias of the "reset" command of the type
		const char * const CMD_PS_RESET_ALIAS = "rset";

		//! The alias of the "set slope" command of the type
		const char * const CMD_PS_SET_SLOPE_ALIAS = "sslp";
		//! The alias of the slope up parameter
		const char * const CMD_PS_SET_SLOPE_UP = "sslp_up";
		//! The alias of the slope down parameter
		const char * const CMD_PS_SET_SLOPE_DOWN = "sslp_down";
		
		//! The alias of the "set slope" command of the type
		const char * const CMD_PS_SET_CURRENT_ALIAS = "sett";
		//! The alias of the current to set
		const char * const CMD_PS_SET_CURRENT = "sett_cur";
        
            //! The alias of the "set polarity" command
		const char * const CMD_PS_SET_POLARITY_ALIAS = "pola";
            //! the type of polarity
		const char * const CMD_PS_SET_POLARITY_VALUE = "pola_value";
                
        const char * const CMD_PS_CALIBRATE = "calibrate";
        const char * const CMD_PS_CALIBRATE_FROM = "from";
        const char * const CMD_PS_CALIBRATE_TO = "to";
        const char * const CMD_PS_CALIBRATE_STEPS = "steps";

        #define DEFAULT_COMMAND_TIMEOUT_MS   5000
        #define DEFAULT_RAMP_TIME_OFFSET_MS  2000

	}
}
#endif
