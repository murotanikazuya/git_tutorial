# Copyright (C) 2010 Olivier Stasse, JRL, CNRS, 2010
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# This files comes from the CMake FAQ: http://www.cmake.org/Wiki/CMake_FAQ
cmake_policy(SET CMP0007 OLD)
IF(NOT EXISTS "/home/murotani/ethercat/ecmaster_integration/build-robot_console-Desktop-Default/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest:\"/home/murotani/ethercat/ecmaster_integration/build-robot_console-Desktop-Default/install_manifest.txt\"")
ENDIF(NOT EXISTS "/home/murotani/ethercat/ecmaster_integration/build-robot_console-Desktop-Default/install_manifest.txt")

MESSAGE(STATUS "catkin path: /usr/local/.catkin")
IF(EXISTS "/usr/local/.catkin")
  MESSAGE(STATUS "Try to remove /usr/local/.catkin")
  EXECUTE_PROCESS(
      COMMAND /home/murotani/Desktop/cmake-3.6.2/bin/cmake -E remove "/usr/local/.catkin"
      RESULT_VARIABLE rm_resval
      OUTPUT_VARIABLE rm_out
      ERROR_VARIABLE rm_err
       )
ENDIF(EXISTS "/usr/local/.catkin")


FILE(READ "/home/murotani/ethercat/ecmaster_integration/build-robot_console-Desktop-Default/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
LIST(REVERSE files)
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  IF(EXISTS "$ENV{DESTDIR}${file}")
    EXECUTE_PROCESS(
      COMMAND /home/murotani/Desktop/cmake-3.6.2/bin/cmake -E remove "$ENV{DESTDIR}${file}"
      RESULT_VARIABLE rm_resval
      OUTPUT_VARIABLE rm_out
      )
    IF(NOT ${rm_resval} STREQUAL 0)
      MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    ENDIF(NOT ${rm_resval} STREQUAL 0)
  ELSE(EXISTS "$ENV{DESTDIR}${file}")
    MESSAGE(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")

    # If file is a broken symbolic link, EXISTS returns false.
    # Workaround this bug by attempting to remove the file anyway.
    EXECUTE_PROCESS(
      COMMAND /home/murotani/Desktop/cmake-3.6.2/bin/cmake -E remove "$ENV{DESTDIR}${file}"
      RESULT_VARIABLE rm_resval
      OUTPUT_VARIABLE rm_out
      ERROR_QUIET
      )
  ENDIF(EXISTS "$ENV{DESTDIR}${file}")
ENDFOREACH(file)
