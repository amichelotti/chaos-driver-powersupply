#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <boost/program_options.hpp>

#include "TestPowerSupply.h"

namespace po = boost::program_options;

int main (int argc, char* argv[] )
{
  std::string mdsServer;
    int debug;
  std::vector<std::string> arrCU;
  std::vector<TestPowerSupply*> arrTest;
    po::options_description desc("options");

    desc.add_options()("help","help");
    desc.add_options()("mds",po::value<std::string>(&mdsServer)->default_value(std::string("mdsserver:5000")),"mds server");
    desc.add_options()("supply,s",po::value<std::vector<std::string> > (&arrCU),"power supply ID to test");
    desc.add_options()("debug,d",po::value<int>(&debug)->default_value(0),"enable debug level");

    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc),vm);
    
    if(vm.count("help")){
        std::cout<<desc<<std::endl;
        return 0;
    }
    if(vm.count("mds")==0){
        std::cout<<"## you must specify a valid metadataserver --mds <server:5000>"<<std::endl;
        return -2;
    }
    if(vm.count("supply")==0){
        std::cout<<"## you must specify a valid CU"<<std::endl;
        return -3;
    }
    mdsServer = vm["mds"].as<std::string>();
    arrCU =vm["supply"].as<std::vector<std::string> >();
    std::cout<<"using MDS: "<<mdsServer<<std::endl;
    
    for(std::vector<std::string>::iterator i=arrCU.begin();i!=arrCU.end();i++){
        TestPowerSupply*p =new TestPowerSupply(*i,mdsServer,vm["debug"].as<int>());
        if(p==NULL){
            std::cout<<"## cannot allocate resources for testing: "<<*i<<std::endl;
            return -4;
        }
        if(p->init()!=0){
            std::cout<<"## cannot initialize device: "<<*i<<std::endl;
            return -5;
        }
        //name, pointer, function, timeout, test repetition
        p->addTest("initTest",p, &TestPowerSupply::initTest,10000,1);
        p->addTest("startTest",p, &TestPowerSupply::startTest,10000,1);
        p->addTest("aliveTest",p, &TestPowerSupply::aliveTest,10000,10);
        p->addTest("power on",p,&TestPowerSupply::onTest,60000,1);
        //name, pointer, function with pointer, value, timeout, test repetition

        p->addTest<float>("Current 50",p,&TestPowerSupply::setCurrentTest,50,60000,1);
        p->addTest<float>("Current 0",p,&TestPowerSupply::setCurrentTest,0,60000,1);
        p->addTest("Standby",p,&TestPowerSupply::standByTest,60000,1);
        p->addTest<int>("Polarity Neg",p,&TestPowerSupply::setPolarityTest,-1,60000,1);
        p->addTest<int>("Polarity Open",p,&TestPowerSupply::setPolarityTest,0,60000,1);
        p->addTest<int>("Polarity Pos",p,&TestPowerSupply::setPolarityTest,1,60000,1);
        p->addTest("poweron 2",p,&TestPowerSupply::onTest,60000,1);
        p->addTest<float>("Current 70",p,&TestPowerSupply::setCurrentTest,70,60000,1);
        p->addTest<float>("Current0 last",p,&TestPowerSupply::setCurrentTest,0,60000,1);
        p->addTest("Standby2",p,&TestPowerSupply::standByTest,60000,1);
        
        p->addTest("stopTest",p, &TestPowerSupply::stopTest,10000,1);
        p->addTest("deinitTest",p, &TestPowerSupply::deinitTest,10000,1);

        arrTest.push_back(p);
    }
    
    for(std::vector<TestPowerSupply*>::iterator i = arrTest.begin();i!=arrTest.end();i++){
        (*i)->runTestsBackGround(1,10);
    }
    
    for(std::vector<TestPowerSupply*>::iterator i = arrTest.begin();i!=arrTest.end();i++){
        std::string filename=(*i)->getCUname() + "_report.txt";
        (*i)->report((char*)filename.c_str());
    }
  return 0;
}

