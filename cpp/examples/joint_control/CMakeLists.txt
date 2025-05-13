cmake_minimum_required (VERSION 3.10.2)
project (joint_control)

# no arm squat example
add_executable(noarm_squat 
    ${CMAKE_CURRENT_SOURCE_DIR}/noarm_squat.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/joint_api_helper.cpp
)
# wiggle arm example
add_executable(wiggle_arm 
    ${CMAKE_CURRENT_SOURCE_DIR}/wiggle_arm_example.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/joint_api_helper.cpp
)

target_compile_features(noarm_squat PUBLIC cxx_std_17)
target_compile_features(wiggle_arm PUBLIC cxx_std_17)

target_include_directories(noarm_squat PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROTOBUF_INCLUDE_DIR}
)

target_include_directories(wiggle_arm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROTOBUF_INCLUDE_DIR}
)

target_link_libraries(noarm_squat PUBLIC bosdyn_client_static)
target_link_libraries(wiggle_arm PUBLIC bosdyn_client_static)

install(TARGETS noarm_squat DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS wiggle_arm DESTINATION ${CMAKE_INSTALL_BINDIR})