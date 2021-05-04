cmake_minimum_required(VERSION 3.16.3)

#####################################   LOCAL VARIABLES   #########################################
set(LOCAL_TARGET Mock_MAX30102)
set(SOURCE_DIR .)
set(HEADER_DIR .)
set(MAIN_APP_DIR ../../../..)



#####################################   CREATE MOCK LIBRARY TARGET   ##############################
add_library(${LOCAL_TARGET}
    ${SOURCE_DIR}/Mock_MAX30102.cpp
)

target_include_directories(${LOCAL_TARGET} PUBLIC
    ${HEADER_DIR}
    ${MAIN_APP_DIR}/Core/Inc
)

target_link_libraries(${LOCAL_TARGET} PUBLIC
    gmock_main
    Mock_HAL_Drivers
)