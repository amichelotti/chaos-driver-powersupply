#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chaos/common/chaos_constants.h>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "TestPowerSupply.h"

#define TEST_DELIMITATOR "|"
#define TEST_NAME_VAL_DELIMITATOR ":"

int main (int argc, char* argv[] )
{
    int debug,loop,keep;
    std::string pattern;
    std::vector<std::string> arrCU;
    std::vector<TestPowerSupply*> arrTest;
    int ret=0;
    
    chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("supply,s",po::value<std::vector<std::string> > (&arrCU),"power supply ID to test");
    
    
    chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("loop,l",po::value<int>(&loop)->default_value(1),"number of test loop to do");
    chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("keep,k",po::value<int>(&keep)->default_value(0),"continue on error");
    chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("test_pattern,t", po::value<std::string>(&pattern),"[str:...:str](|[str:...:str]) the pattern for the tests sequence(init,start, alive, standby, polarity, power_on, test_on, set_current, stop, deinit)");
    
    chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);
    
    if(chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption("supply")==0){
        std::cout<<"## you must specify a valid CU"<<std::endl;
        return -3;
    }
    arrCU=chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<std::vector<std::string> > ("supply");
    loop = chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<int> ("loop");
    keep = chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<int> ("keep");
    std::cout<<"using MDS: "<<chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<std::string>("metadata-server")<<std::endl;
    
    for(std::vector<std::string>::iterator i=arrCU.begin();i!=arrCU.end();i++){
        TestPowerSupply*p =new TestPowerSupply(*i);
        if(p==NULL){
            std::cout<<"## cannot allocate resources for testing: "<<*i<<std::endl;
            return -4;
        }
        if(p->init()!=0){
            std::cout<<"## cannot initialize device: "<<*i<<std::endl;
            return -5;
        }
        if(chaos::ui::ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption("test_pattern")){
            typedef boost::split_iterator<string::iterator> string_split_iterator;
            for(string_split_iterator it = make_split_iterator(pattern, first_finder(TEST_DELIMITATOR, is_iequal()));
                it != string_split_iterator();
                ++it) {
                std::string test_pattern_element = copy_range<std::string>(*it);
                LAPP_ << "Add test parameter:" << test_pattern_element;
                std::vector<std::string> split_vec;
                boost::split( split_vec, test_pattern_element, is_any_of(":"), token_compress_on );
                
                try{
                    if (split_vec[0].compare("init") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern init [init:timeout]";
                            continue;
                        }
                        p->addTest("init",p, &TestPowerSupply::initTest,boost::lexical_cast<int>(split_vec[1]),1);
                    } else if (split_vec[0].compare("start") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern start [start:timeout]";
                            continue;
                        }
                        p->addTest("start",p, &TestPowerSupply::startTest, boost::lexical_cast<int>(split_vec[1]),1);
                    } else if (split_vec[0].compare("alive") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern alive [alive:timeout]";
                            continue;
                        }
                        p->addTest("alive",p, &TestPowerSupply::aliveTest, boost::lexical_cast<int>(split_vec[1]),10);
                    } else if (split_vec[0].compare("standby") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern standby [standby:timeout]";
                            continue;
                        }
                        p->addTest("standby",p, &TestPowerSupply::standByTest, boost::lexical_cast<int>(split_vec[1]),1);
                    } else if (split_vec[0].compare("polarity") == 0) {
                        if(split_vec.size() != 3) {
                            LERR_ << "Bad format for test pattern polarity [polarity:(-1,0,1):timeout]";
                            continue;
                        }
                        p->addTest<int>("polarity",p, &TestPowerSupply::setPolarityTest, boost::lexical_cast<int>(split_vec[1]), boost::lexical_cast<int>(split_vec[2]),1);
                    } else if (split_vec[0].compare("power_on") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern power_on [power_on:timeout]";
                            continue;
                        }
                        p->addTest("power_on",p, &TestPowerSupply::onTest,boost::lexical_cast<int>(split_vec[1]),1);
                    } else if (split_vec[0].compare("test_on") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern test_on [test_on:timeout]";
                            continue;
                        }
                        p->addTest("test_on",p, &TestPowerSupply::onTest,boost::lexical_cast<float>(split_vec[1]),1);
                    } else if (split_vec[0].compare("set_current") == 0) {
                        if(split_vec.size() != 3) {
                            LERR_ << "Bad format for test pattern set_current [set_current:float:timeout]";
                            continue;
                        }
                        p->addTest<float>("set_current",p, &TestPowerSupply::setCurrentTest, boost::lexical_cast<float>(split_vec[1]),boost::lexical_cast<float>(split_vec[2]),1);
                    } else if (split_vec[0].compare("stop") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern set_current [set_current:float:timeout]";
                            continue;
                        }
                        p->addTest("stop",p, &TestPowerSupply::stopTest, boost::lexical_cast<int>(split_vec[1]),1);
                    } else if (split_vec[0].compare("deinit") == 0) {
                        if(split_vec.size() != 2) {
                            LERR_ << "Bad format for test pattern set_current [set_current:float:timeout]";
                            continue;
                        }
                        p->addTest("deinit",p, &TestPowerSupply::deinitTest, boost::lexical_cast<int>(split_vec[1]),1);
                    }
                } catch(...) {
                    LERR_ << "Error adding test for bad format value";
                }
            }
        }else {
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
        }
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
        LAPP_<<"Test powersupply FAILED"<<endl;
    } else {
        LAPP_<<"Test powersupply OK"<<endl;
    }
    return ret;
}

