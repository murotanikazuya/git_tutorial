project(HydraPosControl)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-std=gnu++0x")

# autoconf to find files for libxml++
#find_package(PkgConfig)
#pkg_check_modules(LIBXMLPP REQUIRED libxml++-2.6)

aux_source_directory(. SRC_LIST)
aux_source_directory(../../hydra_common SRC_LIST)
include_directories(../../hydra_common)
include_directories(../../common)
include_directories(../../EC-Master_SDK/INC)
include_directories(../../EC-Master_SDK/INC/Linux)
include_directories(/home/kaminaga/choreonoid/thirdparty/irrxml-1.2/src)
include_directories(/usr/include/eigen3)
include_directories(/usr/local/include/choreonoid-1.5)
include_directories(/usr/local/include/jrtplib3)

# use this to use libxml++
#include_directories(${LIBXMLPP_INCLUDE_DIRS})

add_library(${PROJECT_NAME} ${SRC_LIST})

# use this to link to libxml++
#target_link_libraries(${PROJECT_NAME} ${LIBXMLPP_LIBRARIES})

