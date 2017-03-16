project(HydraConsole)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

set(PROJECT_DESCRIPTION "temp")
set(PROJECT_URL "")

aux_source_directory(. SRC_LIST)
set(SRC_LIST ${SRC_LIST}
    ../hydra_common/CalcForceTorque.cpp
    ../hydra_common/CalcParallelJoint.cpp
    ../hydra_common/chydradatamanager.cpp
    ../hydra_common/chydrajrtpcomm.cpp
    ../hydra_common/chydrashmclient.cpp
    ../hydra_common/kinematics/chydrakinematics.cpp)

include_directories(../common)
include_directories(../hydra_common)
include_directories(../EC-Master_SDK/INC)
include_directories(../EC-Master_SDK/INC/Linux)
include_directories(/usr/include/eigen3)
include_directories(/usr/local/include/jrtplib3)

include(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/eigen.cmake)
include(cmake/ros.cmake)
include(cmake/GNUInstallDirs.cmake)
include(cmake/python.cmake)


#SET(BOOST_COMPONENTS filesystem system thread)
SEARCH_FOR_EIGEN()
SEARCH_FOR_BOOST()

set(CXX_DISABLE_WERROR True)
SETUP_PROJECT()
set(PKG_CONFIG_ADDITIONAL_VARIABLES
  ${PKG_CONFIG_ADDITIONAL_VARIABLES}
  plugindirname
  plugindir
  )


add_required_dependency(roscpp)
add_required_dependency(tf)

add_executable(${PROJECT_NAME}
    ${SRC_LIST}
    )
pkg_config_use_dependency(${PROJECT_NAME} roscpp tf)
target_link_libraries(${PROJECT_NAME} m jrtp jthread pthread ncurses
    tf ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} roscpp
    )

SETUP_PROJECT_FINALIZE()
