cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

add_definitions(-DCHAOS -DDEBUG)

SET(PowerSupplydriver_src core/ChaosPowerSupplyInterface.cpp models/GenericPowerSupplyDD.cpp core/ChaosUIPowersupplyCWrapper.cpp)
SET(PowerSupplycu_src source/powersupply_cu.cpp source/SCPowerSupplyControlUnit.cpp source/CmdPSDefault.cpp source/CmdPSMode.cpp source/CmdPSReset.cpp source/CmdPSSetSlope.cpp source/CmdPSSetCurrent.cpp source/CmdSetPolarity.cpp source/AbstractPowerSupplyCommand.cpp )


IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(driver_powersupply STATIC ${PowerSupplydriver_src})
ELSE()

ADD_LIBRARY(driver_powersupply SHARED ${PowerSupplydriver_src})
ENDIF()

ADD_EXECUTABLE(powersupply_cu ${PowerSupplycu_src})
ADD_EXECUTABLE(powersupply_test test/test_power_supply.cpp test/TestPowerSupply.cpp)

SET(FrameworkLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(FrameworkLib)

TARGET_LINK_LIBRARIES(powersupply_cu pthread driver_powersupply common_powersupply common_serial common_debug chaos_cutoolkit chaos_common ${FrameworkLib})

TARGET_LINK_LIBRARIES(driver_powersupply pthread common_powersupply common_serial common_debug chaos_cutoolkit chaos_uitoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(powersupply_test common_test chaos_uitoolkit driver_powersupply chaos_common common_debug ${FrameworkLib})

INSTALL_TARGETS(/bin powersupply_cu)
INSTALL_TARGETS(/bin powersupply_test)
INSTALL_TARGETS(/lib driver_powersupply)

FILE(GLOB core_src core/*.h)
FILE(GLOB models_src models/*.h)

INSTALL(FILES ${core_src} DESTINATION include/driver/powersupply/core)
INSTALL(FILES ${models_src} DESTINATION include/driver/powersupply/models)
