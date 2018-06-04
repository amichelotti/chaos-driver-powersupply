cmake_minimum_required(VERSION 2.8)
project(chaos_driver_powersupply_hazemeyer)
SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} hazemeyer_powersupply HZLib PARENT_SCOPE)

FILE(GLOB src *.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})
ELSE()
ADD_LIBRARY(${PROJECT_NAME} SHARED ${src})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_powersupply hazemeyer_powersupply HZLib common_modbus modbus chaos_driver_powersupply)
ENDIF()

INSTALL_TARGETS(/lib ${PROJECT_NAME} )





