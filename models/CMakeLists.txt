cmake_minimum_required(VERSION 2.8)
add_subdirectory(Ocem)
add_subdirectory(Hazemeyer)
add_subdirectory(PowerSim)

SET(DRIVERS_LIB ${DRIVERS_LIB} PARENT_SCOPE)
