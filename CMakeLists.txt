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
add_definitions(-DBOOST_ALL_DYN_LINK)

SET(PowerSupplydriver_src source/PowerSupplyDriver.cpp source/PowerSupplyDriverSwitch.cpp)
SET(PowerSupplycu_src source/main.cpp source/PowerSupplyControlUnit.cpp ${PowerSupplydriver_src})

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/../../usr/local/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../chaosframework/usr/local/lib)

ADD_EXECUTABLE(PowerSupplyControlUnit ${PowerSupplycu_src})
ADD_LIBRARY(PowerSupplyDriver SHARED ${PowerSupplydriver_src})
set_target_properties(PowerSupplyDriver PROPERTIES PREFIX "" SUFFIX ".extension")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(PowerSupplyControlUnit pthread chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
	TARGET_LINK_LIBRARIES(PowerSupplyDriver pthread chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(PowerSupplyControlUnit chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
	TARGET_LINK_LIBRARIES(PowerSupplyDriver chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


INSTALL_TARGETS(/bin/PowerSupply PowerSupplyControlUnit)
INSTALL_TARGETS(/extension PowerSupplyDriver)
