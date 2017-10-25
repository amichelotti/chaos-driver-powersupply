cmake_minimum_required(VERSION 2.8)
project(chaos_driver_powersupply)
SET(CHAOS_CUS core/SCPowerSupplyControlUnit.cpp)

FILE(GLOB core_src core/*.cpp)
SET(DRIVERS_LIB )
add_subdirectory(models)

ADD_EXECUTABLE(powersupply_cu test/powersupply_cu.cpp )
ADD_EXECUTABLE(powersupply_test test/test_power_supply.cpp test/TestPowerSupply.cpp )


IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${core_src})
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ELSE()
ADD_LIBRARY(${PROJECT_NAME}  SHARED ${core_src})
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_powersupply chaos_uitoolkit chaos_common chaos_cutoolkit ${FrameworkLib})
TARGET_LINK_LIBRARIES(powersupply_cu ${DRIVERS_LIB} ${PROJECT_NAME} common_serial_ocem common_serial common_debug common_modbus modbus pthread)
TARGET_LINK_LIBRARIES(powersupply_test ${DRIVERS_LIB} ${PROJECT_NAME} common_serial_ocem common_test common_serial common_debug common_modbus modbus pthread)


INSTALL_TARGETS(/bin powersupply_cu)
INSTALL_TARGETS(/bin powersupply_test)
INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL(FILES conf/SCPowerSupplyControlUnit.conf DESTINATION etc/test/powersupply/conf)

