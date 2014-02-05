//
//  TestPowerSupply.h
//  PowerSupply
//
//  Created by andrea michelotti on 29/01/14.
//  Copyright (c) 2014 infn. All rights reserved.
//
#include <iostream>
#include <string>
#include <common/test/CUTest.h>

#ifndef __PowerSupply__TestPowerSupply__
#define __PowerSupply__TestPowerSupply__

class TestPowerSupply:public CUTest<TestPowerSupply> {

   
public:
    TestPowerSupply(std::string cu_name,std::string mds_name,int debuglevel=0):CUTest<TestPowerSupply>(cu_name,mds_name,debuglevel){}
    
    int onTest(void);
    int standByTest(void);
    int setCurrentTest(float*val);
    int setPolarityTest(int* pol);
    
};

#endif /* defined(__PowerSupply__TestPowerSupply__) */
