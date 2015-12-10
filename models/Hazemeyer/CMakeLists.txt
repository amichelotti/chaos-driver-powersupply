cmake_minimum_required(VERSION 2.6)
project(Chaos_Hazemeyer)
SET(DRIVERS_LIB ${DRIVERS_LIB} chaos_driver_powersupply_hazemeyer PARENT_SCOPE)

message(STATUS "Hazemeyer driver ${DRIVERS_LIB}")
add_definitions(-DCHAOS -DDEBUG)
FILE(GLOB src *.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(chaos_driver_powersupply_hazemeyer STATIC ${src})
ELSE()
ADD_LIBRARY(chaos_driver_powersupply_hazemeyer SHARED ${src})
ENDIF()
TARGET_LINK_LIBRARIES(chaos_driver_powersupply_hazemeyer common_powersupply)
INSTALL_TARGETS(/lib chaos_driver_powersupply_hazemeyer)





