cmake_minimum_required(VERSION 2.6)
option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)


IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
    MESSAGE(STATUS "Enabling 32 bit Compilation")
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()

set (CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../../usr/local)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

ADD_DEFINITIONS(-g -O2 -Wall)
add_definitions(-DBOOST_ALL_DYN_LINK -DCHAOS -DDEBUG)

SET(PowerSupplydriver_src core/ChaosPowerSupplyInterface.cpp models/GenericPowerSupplyDD.cpp core/ChaosUIPowersupplyCWrapper.cpp)
SET(PowerSupplycu_src test/powersupply_cu.cpp test/SCPowerSupplyControlUnit.cpp test/CmdPSDefault.cpp test/CmdPSMode.cpp test/CmdPSReset.cpp test/CmdPSSetSlope.cpp test/CmdPSSetCurrent.cpp test/CmdSetPolarity.cpp test/AbstractPowerSupplyCommand.cpp )

INCLUDE_DIRECTORIES(../../ ${PROJECT_SOURCE_DIR}/../../usr/local/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../chaosframework/usr/local/lib)

ADD_EXECUTABLE(powersupply_cu ${PowerSupplycu_src})
ADD_EXECUTABLE(test_powersupply_ui ocem_test_ui/test_power_supply.cpp ocem_test_ui/TestPowerSupply.cpp)
ADD_LIBRARY(driver_powersupply SHARED ${PowerSupplydriver_src})
# set_target_properties(ocem_cu PROPERTIES PREFIX "" SUFFIX ".extension")


SET(FrameworkLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(FrameworkLib)

TARGET_LINK_LIBRARIES(powersupply_cu pthread driver_powersupply common_powersupply common_serial common_debug chaos_cutoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(driver_powersupply pthread common_powersupply common_serial common_debug chaos_cutoolkit chaos_uitoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(test_powersupply_ui common_test chaos_uitoolkit driver_powersupply chaos_common common_debug ${FrameworkLib})

INSTALL_TARGETS(/bin powersupply_cu)
INSTALL_TARGETS(/bin test_powersupply_ui)
INSTALL_TARGETS(/lib driver_powersupply)
FILE(GLOB core_src core/*.h)
FILE(GLOB models_src models/*.h)

INSTALL(FILES ${core_src} DESTINATION include/driver/powersupply/core)
INSTALL(FILES ${models_src} DESTINATION include/driver/powersupply/models)
