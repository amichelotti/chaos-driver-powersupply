cmake_minimum_required(VERSION 2.8)
project(chaos_driver_powersupply_dante_dd)

SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} ${PROJECT_NAME} PARENT_SCOPE)

message(STATUS "DANTE driver ${DRIVERS_LIB}")
SET( src DanteDD.cpp PSDanteDriver.cpp)
IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})
ELSE()
ADD_LIBRARY(${PROJECT_NAME} SHARED ${src})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_powersupply_ocem common_serial_ocem chaos_driver_powersupply chaos_driver_data_import_dante)
ENDIF()
FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/driver/powersupply/models/dante)

#INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)




