/*
 *	ChaosUIPowersupplyCWrapper
 *	!CHOAS
 *	Andrea Michelotti.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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


#ifndef CHAOSFramework_UIToolkitCWrapper_h
#define CHAOSFramework_UIToolkitCWrapper_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  
  /*!
    initialise powersupply
  */
  int initPowerSupply(const char* mds,const char* name, unsigned int* devID);
  int deinitPowerSupply(unsigned int devID);
  int getCurrent(unsigned int devID,double*current);
  int getState(unsigned int devID,int*state);
  int setCurrent(unsigned int devID,float curr);
  int setPol(unsigned int devID,int pol);
  int resetAlarms(unsigned int devID);
  int standby(unsigned int devID);
  int on(unsigned int devID);
  int getPol(unsigned int devID,int*pol);
  int getLive(unsigned int devID,long long *live);
  int getAlarms(unsigned int devID,long long *alrm);
#ifdef __cplusplus
}
#endif


#endif
