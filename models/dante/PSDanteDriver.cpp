#include "PSDanteDriver.h"
#include <stdint.h>

namespace chaos
{
namespace driver
{
namespace powersupply
{

#define DANTE_INFO INFO_LOG(PSDanteDriver)
#define DANTE_DBG DBG_LOG(PSDanteDriver)
#define DANTE_ERR ERR_LOG(PSDanteDriver)

std::string PSDanteDriver::protocol2String(int32_t t){
    switch(t){
        case 0:
            return "SYS8X00";
        case 1:
            return "E642";
        case 2:
            return "Modbus";
        case 3:
            return "Probus";
        case 4:
            return "VSP";
        case 5:
            return "Genesys";
        case 6:
            return "CAENels";
        case 7:
            return "Modbus";
        default:
            return "UNDEFINED";


    }
}

PSDanteDriver::PSDanteDriver(const char *initParameter) : current(0)
{
    dante.driverInit(initParameter);
    // no w accces static DS to get type and other information
    int32_t elementType;
    int ret=dante.getData("elemType",&elementType,::driver::data_import::DanteDriver::STATIC);
    if(ret!=0){
		throw chaos::CException(1, "Cannot fetch STATIC info", __PRETTY_FUNCTION__);
    }
    protocol=elementType&0xF;
    interfaceType=(elementType>>8)&0xF;
    polarityType=(elementType>>16)&0xF;
    alarmType=(elementType>>24)&0xF;

}
PSDanteDriver::~PSDanteDriver()
{
    dante.driverDeinit();
}

int PSDanteDriver::setPolarity(int pol, uint32_t timeo_ms)
{
    DANTE_DBG << "NOT IMPLEMENTED ";

    return 0;
}

int PSDanteDriver::getPolarity(int *_pol, uint32_t timeo_ms)
{
    int32_t pol = 0;
    int ret = dante.getData("outputPolarity", (void *)&pol);
    *_pol = pol;
   
    return ret;
}

int PSDanteDriver::setCurrentSP(float _current, uint32_t timeo_ms)
{
    current = _current;
    return 0;
}
int PSDanteDriver::getCurrentSP(float *_current, uint32_t timeo_ms)
{
    *_current = current;
    double curr = 0;
    int ret = dante.getData("currentSetting", (void *)&curr);
    *_current = curr;
    
    return ret;
}

int PSDanteDriver::startCurrentRamp(uint32_t timeo_ms)
{
    chaos::common::data::CDataWrapper cd;
    cd.addDoubleValue("value", current);
    chaos::common::data::CDWUniquePtr ret = dante.postData("setCurrent", &cd);
    if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR))
    {
        chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
        DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
        return -1;
    }
    return 0;
}

int PSDanteDriver::getVoltageOutput(float *volt, uint32_t timeo_ms)
{
    double curr = 0;
    int ret = dante.getData("outputVolt", (void *)&curr);
    *volt = curr;
    
    return ret;
}

int PSDanteDriver::getCurrentOutput(float *_curr, uint32_t timeo_m)
{
    double curr = 0;
    int ret = dante.getData("outputCurrent", (void *)&curr);
    *_curr = curr;
    return ret;
}

int PSDanteDriver::setCurrentRampSpeed(float asup, float asdown, uint32_t timeo_ms)
{
    chaos::common::data::CDataWrapper cd;
    cd.addDoubleValue("value", asup);
    chaos::common::data::CDWUniquePtr ret = dante.postData("setSlope", &cd);
    if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR))
    {
        chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
        DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
        return -1;
    }
    return 0;
}

int PSDanteDriver::resetAlarms(uint64_t alrm, uint32_t timeo_ms)
{
    chaos::common::data::CDWUniquePtr ret = dante.postData("resetErrors", NULL);
    if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR))
    {
        chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
        DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
        return -1;
    }
    return 0;
}

int PSDanteDriver::getAlarms(uint64_t *alrm, uint32_t timeo_ms)
{
    uint64_t curr = 0;
    int ret = dante.getData("alarms", (void *)&curr);
    *alrm = curr;
    return ret;
}

int PSDanteDriver::shutdown(uint32_t timeo_ms)
{
    chaos::common::data::CDWUniquePtr ret = dante.postData("powerOff", NULL);
    if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR))
    {
        chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
        DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
        return -1;
    }
    return 0;
}

int PSDanteDriver::standby(uint32_t timeo_ms)
{
    chaos::common::data::CDWUniquePtr ret = dante.postData("standby", NULL);
    if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR))
    {
        chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
        DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
        return -1;
    }
    return 0;
}
int PSDanteDriver::poweron(uint32_t timeo_ms)
{
    chaos::common::data::CDWUniquePtr ret = dante.postData("operational", NULL);
    if (ret.get() && ret->hasKey(::driver::data_import::PROT_ERROR))
    {
        chaos::common::data::CDWUniquePtr rr = ret->getCSDataValue(::driver::data_import::PROT_ERROR);
        DANTE_ERR << " DANTE_ERRor occurred:" << rr->getStringValue("msg");
        return -1;
    }
    return 0;
}
using namespace ::common::powersupply;
int PSDanteDriver::getState(int *state, std::string &desc, uint32_t timeo_ms)
{
    int32_t status;
    bool remote;
    int ret = dante.getData("status", (void *)&status);
    ret = dante.getData("remote", (void *)&remote);
    std::stringstream ss;
    *state=0;
    if(remote==false){
		*state|=POWER_SUPPLY_STATE_LOCAL;
        ss<<"Local|";
    }
    if(state==0){
		*state |=POWER_SUPPLY_STATE_OFF;
        ss<<"Off|";


    } else  if(status==1){
        *state |= POWER_SUPPLY_STATE_STANDBY;
        ss<<"Stby|";

    } else if(status==2){
        *state |= POWER_SUPPLY_STATE_ON;
        ss<<"Operational|";

    } else if(status==3){
        *state |= POWER_SUPPLY_STATE_ALARM;
        ss<<"Alarm|";


    }
   desc=ss.str();
    return 0;
}

int PSDanteDriver::init()
{
    return 0;
}

int PSDanteDriver::deinit()
{
    	return 0;
}

int PSDanteDriver::getSWVersion(std::string &version, uint32_t timeo_ms)
{
    version = "PSDanteDriver 1.0.0" ;
    return 0;
}

int PSDanteDriver::getHWVersion(std::string &version, uint32_t timeo_ms)
{
    version = protocol2String(protocol);
    return 0;
}

int PSDanteDriver::getCurrentSensibility(float *sens)
{
    return 0;
}
int PSDanteDriver::getVoltageSensibility(float *sens)
{
    return 0;
}

int PSDanteDriver::setCurrentSensibility(float sens)
{
    return 0;
}

int PSDanteDriver::setVoltageSensibility(float sens)
{
    return 0;
}
int PSDanteDriver::getMaxMinCurrent(float *max, float *min)
{
    return 0;
}

int PSDanteDriver::getMaxMinVoltage(float *max, float *min)
{
    return 0;
}
int PSDanteDriver::getAlarmDesc(uint64_t *alarm)
{
    return 0;
}

int PSDanteDriver::forceMaxCurrent(float max)
{
    return 0;
}

int PSDanteDriver::forceMaxVoltage(float max)
{
    return 0;
}
uint64_t PSDanteDriver::getFeatures()
{
    if(polarityType==0){
        return ::common::powersupply::POWER_SUPPLY_FEAT_BIPOLAR;
    }
    return ::common::powersupply::POWER_SUPPLY_FEAT_MONOPOLAR;
}



chaos::common::data::CDWUniquePtr PSDanteDriver::getDrvProperties(){
chaos::common::data::CDWUniquePtr ret=dante.getDrvProperties();
DANTE_DBG << "Props:"<<ret->getCompliantJSONString();
  return ret;

 }
                  

} // namespace powersupply
} // namespace driver
} // namespace chaos