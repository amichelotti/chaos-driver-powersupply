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

PSDanteDriver::PSDanteDriver(const char *initParameter) : current(0)
{
    dante.driverInit(initParameter);
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

int PSDanteDriver::getPolarity(int *pol, uint32_t timeo_ms)
{
    DANTE_DBG << "NOT IMPLEMENTED ";

    return 0;
}

int PSDanteDriver::setCurrentSP(float _current, uint32_t timeo_ms)
{
    current = _current;
    return 0;
}
int PSDanteDriver::getCurrentSP(float *_current, uint32_t timeo_ms)
{
    *_current = current;
    return 0;
}

int PSDanteDriver::startCurrentRamp(uint32_t timeo_ms)
{
    chaos::common::data::CDataWrapper cd;
    cd.addDoubleValue("value", current);
    chaos::common::data::CDWUniquePtr ret = dante.postData("setCurrent", &cd);
    return 0;
}

int PSDanteDriver::getVoltageOutput(float *volt, uint32_t timeo_ms)
{
    DANTE_DBG << "NOT IMPLEMENTED ";
    return 0;
}

int PSDanteDriver::getCurrentOutput(float *_curr, uint32_t timeo_m)
{
    double curr = 0;
    int ret = dante.getData("current", (void *)&curr);
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
    chaos::common::data::CDWUniquePtr ret = dante.postData("resetDANTE_ERRors", NULL);
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
    *alrm = 0;
    return 0;
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

int PSDanteDriver::getState(int *state, std::string &desc, uint32_t timeo_ms)
{
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
    version = "1.0.0";
    return 0;
}

int PSDanteDriver::getHWVersion(std::string &version, uint32_t timeo_ms)
{
    version = "1.0.0";
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
    return 0;
}

} // namespace powersupply
} // namespace driver
} // namespace chaos