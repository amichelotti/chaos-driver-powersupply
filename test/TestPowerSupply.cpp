//
//  TestPowerSupply.cpp
//  PowerSupply
//
//  Created by andrea michelotti on 29/01/14.
//  Copyright (c) 2014 infn. All rights reserved.
//
#include "TestPowerSupply.h"
#include <common/debug/debug.h>
#define TEST_TIMEOUT_MS 20*1000
#include <boost/date_time/posix_time/posix_time.hpp>
#include <common/powersupply/core/AbstractPowerSupply.h>
#undef CHAOSFramework_UIToolkitCWrapper_h
#include "driver/powersupply/core/ChaosUIPowersupplyCWrapper.h"

int TestPowerSupply::onTest(){
    int err;
    int state=0;
    boost::posix_time::ptime start= boost::posix_time::microsec_clock::local_time();
    err = on(devID);
    
    if(err!=0){
        return err;
    }
    while(((state&common::powersupply::POWER_SUPPLY_STATE_ON) ==0) && ((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds() < test_timeo)){
        getState(devID, &state);
    }
    if((state&common::powersupply::POWER_SUPPLY_STATE_ON) ==common::powersupply::POWER_SUPPLY_STATE_ON){
        DPRINT("set poweron OK state x%x\n",state);
        return 0;
    }
    
    if(((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds()) >= test_timeo){
        DERR("Timeout setting poweron FAIL state x%x,time elapsed %lld ms\n",state,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());
        return -100;
    }
    DERR("setting poweron FAIL state x%x,time elapsed %lld ms\n",state,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());
 
    return -1;
}

int TestPowerSupply::initTest(){
  int ret=CUTest<TestPowerSupply>::initTest();
  int state;
  if(ret==0){
    int ret = getState(devID,&state);
    if(ret == 0){
      if(state&common::powersupply::POWER_SUPPLY_STATE_STANDBY){
	DPRINT("Power supply is already in standby\n");
	return 0;
      }
      else if(state&common::powersupply::POWER_SUPPLY_STATE_ON){
	float cur=0;
       	DPRINT("Power supply is ON, setting current 0\n");
	ret = setCurrentTest(&cur);
	if(ret == 0){
	  DPRINT("Forcing standby\n");
	  return standByTest();
	}
      }
    }
  }
  return ret;
}
int TestPowerSupply::standByTest(){
    int err;
    int state=0;
    boost::posix_time::ptime start= boost::posix_time::microsec_clock::local_time();
    err = standby(devID);
    DPRINT("setting standby");
    if(err!=0){
        DERR("setting standby, err %d\n",err);

        return err;
    }
    while(((state&common::powersupply::POWER_SUPPLY_STATE_STANDBY) ==0) && ((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds() < test_timeo)){
        getState(devID, &state);
    }
    if((state&common::powersupply::POWER_SUPPLY_STATE_STANDBY) ==common::powersupply::POWER_SUPPLY_STATE_STANDBY){
        DPRINT("set standby OK state x%x\n",state);

        return 0;
    }
    if(((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds()) >= test_timeo){
        DERR("Timeout setting standby FAIL state x%x,time elapsed %lld ms\n",state,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());
        return -100;
    }
    DERR("setting standby FAIL state x%x,time elapsed %lld ms\n",state,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());

    return -1;
}

int TestPowerSupply::setCurrentTest(float*val){
    int err;
    double readout;
    DPRINT("setting current to %f\n",*val);
    err = setCurrent(devID,*val);
    if(err!=0){
        DERR("setting current to %f, err %d\n",*val,err);

        return err;
    }
    boost::posix_time::ptime start=boost::posix_time::microsec_clock::local_time();
    do{
        getCurrent(devID, &readout);
    } while((fabs(readout-*val)>1.0) && (((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds()) < test_timeo));
    if(fabs(readout-*val)<=1.0){
        DPRINT("read current OK %f\n",readout);

        return 0;
    }
    if((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds() >= test_timeo){
        DERR("Timeout Setting current to %f read %f, time elapsed %lld ms\n",*val,readout,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());
        return -100;
    }
    DERR("Setting current to %f read %f, time elapsed %lld ms\n",*val,readout,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());
    return -1;
}

int TestPowerSupply::setPolarityTest(int *pol){
    int err;
    int readpol;
    err = setPol(devID,*pol);
    if(err!=0){
        return err;
    }
    boost::posix_time::ptime start= boost::posix_time::microsec_clock::local_time();
    do{
        getPol(devID, &readpol);
    } while((*pol!=readpol) && (((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds()) < test_timeo));
    if(*pol==readpol){
        DPRINT("read polarity ok \n");
        return 0;
    }
    
    
    if(((boost::posix_time::microsec_clock::local_time()-start).total_milliseconds()) >= test_timeo){
        DERR("Timeout Polarity set %d, polarity read %d, time elapsed %lld\n",*pol,readpol,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());
        return -100;
    }
    DERR("Polarity set %d, polarity read %d, time elapsed %lld\n",*pol,readpol,(boost::posix_time::microsec_clock::local_time()-start).total_milliseconds());
    return -1;
    
}
