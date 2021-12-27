# we require CMake version 3.5 (for iRODS)
cmake_minimum_required(VERSION 3.5)

# set project name
PROJECT(irodsclient)

# find Qt CMake packages
find_package(Qt5 CONFIG REQUIRED Widgets Xml Svg)

# find iRODS packages
find_package(IRODS 4.2.10 EXACT REQUIRED CONFIG)

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

# use auto moc feature to build qt meta objects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include(${IRODS_TARGETS_PATH})

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH ${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")

add_compile_options(-nostdinc++ -Wall -Wextra -Werror -Wno-unused-parameter)
link_libraries(c++abi)
include_directories(${IRODS_EXTERNALS_FULLPATH_CLANG}/include/c++/v1)

# Qt UI compiler files
file(GLOB UI_FILES *.ui)

# project header files
file(GLOB QT_WRAP *.h)

# project source files
file(GLOB CXX_FILES *.cpp)

# use Qt5 CMake UI Compiler wrapping
qt5_wrap_ui(UISrcs ${UI_FILES})

# add Qt resources
qt5_add_resources(UI_RESOURCES icons.qrc)

# configure target executable
add_executable(irodsclient ${CXX_FILES} ${UISrcs} ${QT_WRAP} ${UI_RESOURCES})

# include necessary Qt5 modules for linkage 
qt5_use_modules(irodsclient Core Gui Svg Xml)

target_link_libraries(	    
    irodsclient	    
    irods_client
    irods_plugin_dependencies
    irods_common
    ${IRODS_EXTERNALS_FULLPATH_AVRO}/lib/libavrocpp.so
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_filesystem.so
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_program_options.so
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_system.so
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_thread.so
    ${IRODS_EXTERNALS_FULLPATH_ZMQ}/lib/libzmq.so
    ${IRODS_EXTERNALS_FULLPATH_FMT}/lib/libfmt.so
    pthread
    ssl
    crypto
    )

target_include_directories(
    irodsclient
    PRIVATE
    ${IRODS_INCLUDE_DIRS}
    ${IRODS_EXTERNALS_FULLPATH_AVRO}/include
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
    ${IRODS_EXTERNALS_FULLPATH_ARCHIVE}/include
    ${IRODS_EXTERNALS_FULLPATH_CPPZMQ}/include
    ${IRODS_EXTERNALS_FULLPATH_ZMQ}/include
    ${IRODS_EXTERNALS_FULLPATH_JSON}/include
    ${IRODS_EXTERNALS_FULLPATH_FMT}/include
    )

target_compile_definitions(irodsclient PRIVATE ${IRODS_COMPILE_DEFINITIONS} BOOST_SYSTEM_NO_DEPRECATED)
target_compile_options(irodsclient PRIVATE -Wno-write-strings)
set_property(TARGET irodsclient PROPERTY CXX_STANDARD ${IRODS_CXX_STANDARD})

