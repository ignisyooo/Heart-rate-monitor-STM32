cmake_minimum_required(VERSION 3.16.3)
project(Heart_Rate_Monitor VERSION 0.1.0)
enable_testing()

#####################################   LOCAL VARIABLES   #########################################
set(TESTS_DIR Tests)
set(MOCKS_DIR Mocks)

#####################################   SUBDIRECTORIES   ##########################################
add_subdirectory(googletest)

#####################################   TESTS   ###################################################
list(APPEND TESTS
    MAX30102
    #---add new tests here---
)

#####################################   TEST ADD   ################################################
foreach(TEST IN LISTS TESTS)
    add_subdirectory(${TESTS_DIR}/${TEST} ${TESTS_DIR}/${TEST})
    add_subdirectory(${TESTS_DIR}/${TEST}/mocks ${MOCKS_DIR}/Mock_MAX30102)
endforeach()

add_subdirectory(../Drivers/STM32F3xx_HAL_Driver/Tests/mocks ${MOCKS_DIR}/Mock_HAL_Drivers)
