project(HydraConsole)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
set(CMAKE_BUILD_TYPE Release)
aux_source_directory(. SRC_LIST)
#aux_source_directory(../hydra_common SRC_LIST)
set(SRC_LIST ${SRC_LIST}  ../hydra_common/CalcForceTorque.cpp ../hydra_common/CalcParallelJoint.cpp
../hydra_common/chydradatamanager.cpp ../hydra_common/chydrajrtpcomm.cpp ../hydra_common/chydrashmclient.cpp
../hydra_common/kinematics/chydrakinematics.cpp)
include_directories(../common)
include_directories(../hydra_common)
include_directories(../EC-Master_SDK/INC)
include_directories(../EC-Master_SDK/INC/Linux)
include_directories(/home/kaminaga/choreonoid/thirdparty/irrxml-1.2/src)
include_directories(/usr/include/eigen3)
include_directories(/usr/local/include/jrtplib3)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} m jrtp jthread pthread ncurses)

