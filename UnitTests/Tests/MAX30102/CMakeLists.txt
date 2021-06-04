cmake_minimum_required(VERSION 3.16.3)

#####################################   LOCAL VARIABLES   #########################################
set(LOCAL_TARGET MAX30102_UT)
set(TEST_LIB ${LOCAL_TARGET}lib)
set(APP_DIR ../../..)
set(SOURCE_DIR ${APP_DIR}/Core/Src)
set(HEADER_DIR ${APP_DIR}/Core/Inc)



#####################################   TESTED COMPONENT LIBRARY   #########################################
include_directories(
    stubs/.
    ${HEADER_DIR}
)

add_library(${TEST_LIB}
    ${SOURCE_DIR}/MAX30102.c
    ${SOURCE_DIR}/algorithm.c
    ${SOURCE_DIR}/screen.c
    ${SOURCE_DIR}/SSD1331.c
    ${SOURCE_DIR}/Fonts.c
)

target_link_libraries(${TEST_LIB} PUBLIC
    Mock_HAL_Drivers
)

#####################################   TEST   ####################################################
add_executable(${LOCAL_TARGET}
    MAX30102_UT.cpp
)

target_compile_definitions(${LOCAL_TARGET} PUBLIC
    UNIT_TESTING
)

target_link_libraries(${LOCAL_TARGET} PRIVATE
    ${TEST_LIB}
    gtest_main
    gmock_main
    gcov

    Mock_HAL_Drivers
)

add_test(NAME ${LOCAL_TARGET}
    COMMAND ${LOCAL_TARGET}
)
