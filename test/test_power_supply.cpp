#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chaos/common/chaos_constants.h>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>

#include "TestPowerSupply.h"


int main (int argc, const char* argv[] )
{
  int debug,loop,keep;
  std::vector<std::string> arrCU;
  std::vector<TestPowerSupply*> arrTest;
  int ret=0;

    chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("supply,s",po::value<std::vector<std::string> > (&arrCU),"power supply ID to test");
    
  
    chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("loop,l",po::value<int>(&loop)->default_value(1),"number of test loop to do");
    chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("keep,k",po::value<int>(&keep)->default_value(0),"continue on error");

    chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);

    if(chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption("supply")==0){
        LERR_<<"## you must specify a valid CU";
        return -3;
    }
    if(chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption("metadata-server")==0){
            LERR_<<"## you must specify a valid MDS";
            return -4;
      }

    arrCU=chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<std::vector<std::string> > ("supply");
    loop = chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<int> ("loop");
    keep = chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<int> ("keep");
    LAPP_<<"using MDS: "<<chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<std::string>("metadata-server");
    try{
    for(std::vector<std::string>::iterator i=arrCU.begin();i!=arrCU.end();i++){
        TestPowerSupply*p =new TestPowerSupply(*i);
        if(p==NULL){
            LERR_<<"## cannot allocate resources for testing: "<<*i<<std::endl;
            return -4;
        }
        if(p->init()!=0){
            LERR_<<"## cannot initialize device: "<<*i<<std::endl;
            return -5;
        }
        //name, pointer, function, timeout, test repetition
        p->addTest("initTest",p, &TestPowerSupply::initTest,30000,1);
        p->addTest("startTest",p, &TestPowerSupply::startTest,30000,1);
	p->addTest("aliveTest",p, &TestPowerSupply::aliveTest,30000,10);
	p->addTest("Standby",p,&TestPowerSupply::standByTest,300000,1);
	p->addTest<int>("Polarity Pos",p,&TestPowerSupply::setPolarityTest,1,300000,1);
        p->addTest("power on",p,&TestPowerSupply::onTest,300000,1);
        //name, pointer, function with pointer, value, timeout, test repetition
        p->addTest<float>("Current 50",p,&TestPowerSupply::setCurrentTest,50,300000,1);
        p->addTest<float>("Current 0",p,&TestPowerSupply::setCurrentTest,0,300000,1);
	p->addTest("Standby2",p,&TestPowerSupply::standByTest,300000,1);
	//        p->addTest<int>("Polarity Neg",p,&TestPowerSupply::setPolarityTest,-1,60000,1);
	//        p->addTest<int>("Polarity Open",p,&TestPowerSupply::setPolarityTest,0,60000,1);
        p->addTest<int>("Polarity Pos",p,&TestPowerSupply::setPolarityTest,1,60000,1);
        p->addTest("poweron 2",p,&TestPowerSupply::onTest,300000,1);
        p->addTest<float>("Current 30",p,&TestPowerSupply::setCurrentTest,70,300000,1);
        p->addTest<float>("Current0 last",p,&TestPowerSupply::setCurrentTest,0,300000,1);
        p->addTest("Standby3",p,&TestPowerSupply::standByTest,300000,1);
        
        p->addTest("stopTest",p, &TestPowerSupply::stopTest,100000,1);
        p->addTest("deinitTest",p, &TestPowerSupply::deinitTest,100000,1);

        arrTest.push_back(p);
    }
    
    for(std::vector<TestPowerSupply*>::iterator i = arrTest.begin();i!=arrTest.end();i++){
        (*i)->runTestsBackGround(keep,loop);
        sleep(1);
    }
    
    for(std::vector<TestPowerSupply*>::iterator i = arrTest.begin();i!=arrTest.end();i++){
        std::string filename=(*i)->getCUname() + "_report.txt";
        ret +=(*i)->report((char*)filename.c_str());
    }
    if(ret !=0){
      LAPP_<<"Test powersupply FAILED";
    } else {
      LAPP_<<"Test powersupply OK";
    }
    } catch(chaos::CException e){
        LERR_<<"Exception:" << e.what();
        return e.errorCode;
    }
    return ret;
}

