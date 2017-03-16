# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "dynamic_graph_bridge: 0 messages, 1 services")

set(MSG_I_FLAGS "-Istd_msgs:/opt/ros/indigo/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(genlisp REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(dynamic_graph_bridge_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv" NAME_WE)
add_custom_target(_dynamic_graph_bridge_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "dynamic_graph_bridge" "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv" ""
)

#
#  langs = gencpp;genlisp;genpy
#

### Section generating for lang: gencpp
### Generating Messages

### Generating Services
_generate_srv_cpp(dynamic_graph_bridge
  "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/dynamic_graph_bridge
)

### Generating Module File
_generate_module_cpp(dynamic_graph_bridge
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/dynamic_graph_bridge
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(dynamic_graph_bridge_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(dynamic_graph_bridge_generate_messages dynamic_graph_bridge_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv" NAME_WE)
add_dependencies(dynamic_graph_bridge_generate_messages_cpp _dynamic_graph_bridge_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(dynamic_graph_bridge_gencpp)
add_dependencies(dynamic_graph_bridge_gencpp dynamic_graph_bridge_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS dynamic_graph_bridge_generate_messages_cpp)

### Section generating for lang: genlisp
### Generating Messages

### Generating Services
_generate_srv_lisp(dynamic_graph_bridge
  "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/dynamic_graph_bridge
)

### Generating Module File
_generate_module_lisp(dynamic_graph_bridge
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/dynamic_graph_bridge
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(dynamic_graph_bridge_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(dynamic_graph_bridge_generate_messages dynamic_graph_bridge_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv" NAME_WE)
add_dependencies(dynamic_graph_bridge_generate_messages_lisp _dynamic_graph_bridge_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(dynamic_graph_bridge_genlisp)
add_dependencies(dynamic_graph_bridge_genlisp dynamic_graph_bridge_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS dynamic_graph_bridge_generate_messages_lisp)

### Section generating for lang: genpy
### Generating Messages

### Generating Services
_generate_srv_py(dynamic_graph_bridge
  "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/dynamic_graph_bridge
)

### Generating Module File
_generate_module_py(dynamic_graph_bridge
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/dynamic_graph_bridge
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(dynamic_graph_bridge_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(dynamic_graph_bridge_generate_messages dynamic_graph_bridge_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/opt/openrobots/robotpkg/wip/dynamic-graph-bridge-v3/work.ubuntu/dynamic_graph_bridge-v3-3.0.5/srv/RunPythonFile.srv" NAME_WE)
add_dependencies(dynamic_graph_bridge_generate_messages_py _dynamic_graph_bridge_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(dynamic_graph_bridge_genpy)
add_dependencies(dynamic_graph_bridge_genpy dynamic_graph_bridge_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS dynamic_graph_bridge_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/dynamic_graph_bridge)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/dynamic_graph_bridge
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
add_dependencies(dynamic_graph_bridge_generate_messages_cpp std_msgs_generate_messages_cpp)

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/dynamic_graph_bridge)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/dynamic_graph_bridge
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
add_dependencies(dynamic_graph_bridge_generate_messages_lisp std_msgs_generate_messages_lisp)

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/dynamic_graph_bridge)
  install(CODE "execute_process(COMMAND \"/usr/bin/python2.7\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/dynamic_graph_bridge\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/dynamic_graph_bridge
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
add_dependencies(dynamic_graph_bridge_generate_messages_py std_msgs_generate_messages_py)
