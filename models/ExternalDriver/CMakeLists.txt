cmake_minimum_required(VERSION 2.8)
project(chaos_driver_powersupply_external)
SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} PARENT_SCOPE)

FILE(GLOB src *.cpp)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})

INSTALL_TARGETS(/lib ${PROJECT_NAME} )





