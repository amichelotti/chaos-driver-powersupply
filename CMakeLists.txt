cmake_minimum_required(VERSION 2.6)
project(powersupply)
SET(CHAOS_CUS core/SCPowerSupplyControlUnit.cpp)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

add_definitions(-DCHAOS -DDEBUG)
FILE(GLOB core_src core/*.cpp)
SET(DRIVERS_LIB )
add_subdirectory(models)

MESSAGE(STATUS "Libraries ${DRIVERS_LIB}")
ADD_EXECUTABLE(powersupply_cu test/powersupply_cu.cpp )
ADD_EXECUTABLE(powersupply_test test/test_power_supply.cpp test/TestPowerSupply.cpp )


IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(chaos_driver_powersupply STATIC ${core_src})
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
TARGET_LINK_LIBRARIES(powersupply_cu ${DRIVERS_LIB} chaos_driver_powersupply common_powersupply common_serial common_debug chaos_cutoolkit chaos_uitoolkit ${FrameworkLib})
TARGET_LINK_LIBRARIES(powersupply_test ${DRIVERS_LIB} chaos_driver_powersupply common_test chaos_uitoolkit chaos_driver_powersupply common_debug ${FrameworkLib})
ELSE()
ADD_LIBRARY(chaos_driver_powersupply SHARED ${core_src})
TARGET_LINK_LIBRARIES(powersupply_cu ${DRIVERS_LIB}  chaos_driver_powersupply common_powersupply common_serial common_debug chaos_cutoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(powersupply_test ${DRIVERS_LIB} chaos_driver_powersupply common_test chaos_uitoolkit chaos_common common_debug ${FrameworkLib})
TARGET_LINK_LIBRARIES(chaos_driver_powersupply ${DRIVERS_LIB} common_powersupply common_serial common_debug chaos_cutoolkit chaos_uitoolkit chaos_common ${FrameworkLib})
ENDIF()



INSTALL_TARGETS(/bin powersupply_cu)
INSTALL_TARGETS(/bin powersupply_test)
INSTALL_TARGETS(/lib chaos_driver_powersupply)


