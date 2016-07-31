# we require CMake version 2.8.11 (Qt5 recommended)
cmake_minimum_required(VERSION 2.8.11)

# set project name
PROJECT(irodsclident)

# set custom cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# set CMake configuration variables and default values
set(OpenSSL_DIR /usr/local/opt/openssl CACHE PATH "OpenSSL path")

# find Qt CMake packages
find_package(Qt5 5.4.0 CONFIG REQUIRED Widgets Xml Svg)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # set correct flags for qt on gcc
  SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(iRODS REQUIRED)

include_directories(
  ${OpenSSL_DIR}/include
  ${iRODS_INCLUDE_DIRS}
)

link_directories(
  ${OpenSSL_DIR}/lib
  ${iRODS_LIBS_DIRS}
)

add_subdirectory(src)

install(FILES irodsclient.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
