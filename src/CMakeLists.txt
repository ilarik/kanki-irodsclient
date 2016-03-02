# we require CMake version 2.8.11 (Qt5 recommended)
cmake_minimum_required(VERSION 2.8.11)

# set project name
PROJECT(irodsclident)

# use auto moc feature to build qt meta objects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# set CMake configuration variables and default values
set(OpenSSL_DIR /usr/local/opt/openssl CACHE PATH "OpenSSL path")
set(iRODS_DIR /usr/local/irods CACHE PATH "iRODS PATH")
set(iRODS_Boost boost_1_58_0z CACHE STRING "iRODS Boost version")
set(iRODS_Jansson jansson-2.7 CACHE STRING "iRODS Jansson version")

# find Qt CMake packages
find_package(Qt5 5.4.0 CONFIG REQUIRED Widgets Xml Svg)

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

include_directories(
	${OpenSSL_DIR}/include
	${iRODS_DIR}/external/${iRODS_Boost}
	${iRODS_DIR}/iRODS/lib/core/include
	${iRODS_DIR}/iRODS/lib/api/include
	${iRODS_DIR}/iRODS/lib/hasher/include
	${iRODS_DIR}/iRODS/server/core/include
	${iRODS_DIR}/iRODS/server/icat/include
	${iRODS_DIR}/iRODS/server/drivers/include
	${iRODS_DIR}/iRODS/server/re/include
	${iRODS_DIR}/iRODS/lib/api/include
	${iRODS_DIR}/iRODS/lib/md5/include
	${iRODS_DIR}/iRODS/lib/sha1/include
	${iRODS_DIR}/iRODS/lib/rbudp/include
)

link_directories(
	${OpenSSL_DIR}/lib 
	${iRODS_DIR}/external/${iRODS_Boost}/stage/lib
	${iRODS_DIR}/external/${iRODS_Jansson}/src/.libs
	${iRODS_DIR}/iRODS/lib/core/obj
)

# configure target executable
add_executable(irodsclient MACOSX_BUNDLE ${CXX_FILES} ${UISrcs} ${QT_WRAP} ${UI_RESOURCES})

# include necessary Qt5 modules for linkage 
qt5_use_modules(irodsclient Core Gui Svg Xml)

add_library(jansson STATIC IMPORTED)
set_property(TARGET jansson PROPERTY IMPORTED_LOCATION ${iRODS_DIR}/external/${iRODS_Jansson}/src/.libs/libjansson.a)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# needed for linkage in OSX with iRODS 4.1.8+
add_library(client_exec STATIC IMPORTED)
set_property(TARGET client_exec PROPERTY IMPORTED_LOCATION ${iRODS_DIR}/iRODS/lib/client_exec/obj/irods_client_rule_execution_manager_factory.o)

target_link_libraries(
        irodsclient
	client_exec
	RodsAPIs
        boost_filesystem
        boost_regex
	boost_system
        boost_thread
        boost_chrono
	boost_date_time
        boost_iostreams
        boost_program_options
	jansson
        ssl
        crypto
)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

target_link_libraries(
	irodsclient
	RodsAPIs 
	boost_filesystem
	boost_regex
	boost_system
	boost_thread
	boost_chrono
	boost_date_time
	boost_iostreams
	boost_program_options
	jansson
	ssl
	crypto
)

endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
