cmake_minimum_required(VERSION 2.6)
project(Ocem)
SET(DRIVERS_LIB ${DRIVERS_LIB} chaos_driver_powersupply_ocem_dd PARENT_SCOPE)

message(STATUS "Ocem driver ${DRIVERS_LIB}")
add_definitions(-DCHAOS -DDEBUG)
FILE(GLOB src *.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(chaos_driver_powersupply_ocem_dd STATIC ${src})
ELSE()
ADD_LIBRARY(chaos_driver_powersupply_ocem_dd SHARED ${src})
ENDIF()
TARGET_LINK_LIBRARIES(chaos_driver_powersupply_ocem_dd common_powersupply)
INSTALL_TARGETS(/lib chaos_driver_powersupply_ocem_dd)





