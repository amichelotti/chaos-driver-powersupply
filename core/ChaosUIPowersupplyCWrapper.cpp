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


#ifndef CHAOSUIPowersupplyCWrapper_h
#define CHAOSUIPowersupplyCWrapper_h
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <chaos/ui_toolkit/ChaosUIToolkitCWrapper.h>
#include <stdint.h>
#ifdef CHAOSUIPOWERSUPPLYCWRAPPER_DEBUG
#define DEBUG
#endif

#include "common/debug/debug.h"
#ifdef __cplusplus
extern "C" {
#endif



  int initPowerSupply(const char* mds,const char* name, unsigned int* devID){
    char tmpInitString[256];
    int err;
    static int toolkit_initialised=0;

    if(toolkit_initialised==0){
      sprintf(tmpInitString, "metadata-server=%s\nlog-on-console=true", mds);
      err = initToolkit(tmpInitString);
      if (err != 0) {
	DPRINT("Error initToolkit %d\n", err);
	return -1;
      }
      toolkit_initialised = 1;
      DPRINT("Toolkit initialised \"%s\"\n",tmpInitString);
    }

    err = getNewControllerForDeviceID(name, devID);
    if (err != 0) {
      DPRINT("Error getNewControllerForDeviceID for CU \"%s\" devID @x%x err: %d\n", name,devID,err);
      return -2;
    }

    err = setControllerTimeout((uint32_t)*devID, 100000000);
    if (err != 0) {
      DPRINT("Error setting timeout %d\n", err);
      return -1;
    }

    DPRINT("Init device 0x%x\n",*devID);
    err = initDevice(*devID);
    if (err != 0) {
      DPRINT("Error initDevice %d\n", err);
      // return -1;
    }
    sleep(1);
    DPRINT("Starting device 0x%x\n",*devID);
    err = startDevice(*devID);
    if (err != 0) {
      DPRINT("Error startDevice %d\n", err);
      return -4;
    }


    return 0;
  }

  int deinitPowerSupply(unsigned int devID){
    
    return deinitController((uint32_t)devID);
  }

  static int fetchValue(unsigned int devID,const char*attributeName,char**attributeValue){
    int err;
    DPRINT("Fetching device 0x%x\n",devID);
    err = fetchLiveData(devID);
    if (err != 0) {
      DPRINT("Error fetchLiveData %d\n", err);
      return -1;
    }
    err = getStrValueForAttribute(devID, attributeName, attributeValue);
    if (err != 0) {
      DPRINT("Error getStrValueForAttribute %d for value \"%s\"\n", err, attributeName);
      return -1;
    }
    return 0;
  }

  int getCurrent(unsigned int devID,double*current){
    int err;
    char* attributeValue=NULL;
    err = fetchValue(devID,"current",&attributeValue);
    if(err==0){
      *current = atof(attributeValue);
      DPRINT("got %f\n",*current);
      free(attributeValue);
      return 0;
    }
    DPRINT("Error fetchLiveData %d\n", err);
    return -1;
  }

  int getPol(unsigned int devID,int*pol){
    int err;
    char* attributeValue=NULL;
    err = fetchValue(devID,"polarity",&attributeValue);
    if(err==0){
      *pol = atoi(attributeValue);
      DPRINT("got %d\n",*pol);
      free(attributeValue);
      return 0;
    }
    DPRINT("Error fetchLiveData %d\n", err);
    return -1;
  }

  int getState(unsigned int devID,int*pol){
    int err;
    char* attributeValue=NULL;
    err = fetchValue(devID,"status_id",&attributeValue);
    if(err==0){
      *pol = atoi(attributeValue);
      DPRINT("got %d\n",*pol);
      free(attributeValue);
      return 0;
    }
    DPRINT("Error fetchLiveData %d\n", err);
    return -1;
  }

  int setCurrent(unsigned int devID,float curr){
    int err;
    char parameter[256];
    sprintf(parameter,"{\"sett_cur\":%f}",curr);
    DPRINT("submitting command %s\n",parameter);
    err = submitSlowControlCommand(devID,"sett",
				 1,
				 50,
				 0,
				 0,
				 0 ,
				 parameter);
    return err;
  }
  int setPol(unsigned int devID,int pol){
    int err;
    char stringa[256];
    sprintf(stringa,"{\"pola_value\":%d}",pol);
    DPRINT("submitting command %s\n",stringa);
    err = submitSlowControlCommand(devID,"pola",
				 1,
				 50,
				 0,
				 0,
				 0 ,
				 stringa);
    return err;

  }
  int standby(unsigned int devID){
    int err;
    char stringa[256];
    sprintf(stringa,"{\"mode_type\":0}");
    DPRINT("submitting command %s\n",stringa);
    err = submitSlowControlCommand(devID,"mode",
				 1,
				 50,
				 0,
				 0,
				 0 ,
				 stringa);
    return err;

  }
  int on(unsigned int devID){
    int err;
    char stringa[256];
    sprintf(stringa,"{\"mode_type\":1}");
    DPRINT("submitting command %s\n",stringa);
    err = submitSlowControlCommand(devID,"mode",
				 1,
				 50,
				 0,
				 0,
				 0 ,
				 stringa);
    return err;

  }

  int getLive(unsigned int devID,long long *live) {
    int err;
    char* attributeValue=NULL;
    err = fetchLiveData(devID);
    err= getTimeStamp(devID,(uint64_t*)live);
    DPRINT("Error fetchLiveData %d\n", err);
    return err;
  }


#ifdef __cplusplus
}
#endif


#endif
