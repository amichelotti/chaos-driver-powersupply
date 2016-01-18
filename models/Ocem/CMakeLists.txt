cmake_minimum_required(VERSION 2.6)
project(chaos_driver_powersupply_ocem_dd)

SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} common_powersupply_ocem PARENT_SCOPE)

message(STATUS "Ocem driver ${DRIVERS_LIB}")
add_definitions(-DCHAOS -DDEBUG)
FILE(GLOB src *.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})
ELSE()
ADD_LIBRARY(${PROJECT_NAME} SHARED ${src})
ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_powersupply_ocem common_serial_ocem)
INSTALL_TARGETS(/lib ${PROJECT_NAME})





