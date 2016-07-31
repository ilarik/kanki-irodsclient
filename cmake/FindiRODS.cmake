
set(iRODS_SRC_DIR /usr/local/irods CACHE PATH "iRODS src directory")
set(iRODS_PREFIX /usr/ CACHE PATH "iRODS install PATH")
set(iRODS_Boost boost_1_58_0z CACHE STRING "iRODS Boost version")
set(iRODS_Jansson jansson-2.7 CACHE STRING "iRODS Jansson version")


set(iRODS_INCLUDE_DIRS 	${iRODS_SRC_DIR}/external/${iRODS_Boost}
  ${iRODS_SRC_DIR}/iRODS/lib/core/include
  ${iRODS_SRC_DIR}/iRODS/lib/api/include
  ${iRODS_SRC_DIR}/iRODS/lib/hasher/include
  ${iRODS_SRC_DIR}/iRODS/server/core/include
  ${iRODS_SRC_DIR}/iRODS/server/icat/include
  ${iRODS_SRC_DIR}/iRODS/server/drivers/include
  ${iRODS_SRC_DIR}/iRODS/server/re/include
  ${iRODS_SRC_DIR}/iRODS/lib/api/include
  ${iRODS_SRC_DIR}/iRODS/lib/md5/include
  ${iRODS_SRC_DIR}/iRODS/lib/sha1/include
  ${iRODS_SRC_DIR}/iRODS/lib/rbudp/include
  ${iRODS_PREFIX}/include/irods
  )

 set(iRODS_LIBS_DIRS ${iRODS_SRC_DIR}/external/${iRODS_Boost}/stage/lib
  ${iRODS_SRC_DIR}/external/${iRODS_Jansson}/src/.libs
  ${iRODS_SRC_DIR}/iRODS/lib/core/obj
  ${iRODS_PREFIX}/lib/irods/externals
  )

add_library(jansson STATIC IMPORTED)
set(JANSSON_SRC_LIB "${iRODS_SRC_DIR}/external/${iRODS_Jansson}/src/.libs/libjansson.a")
if(EXISTS "${JANSSON_SRC_LIB}")
  set_property(TARGET jansson PROPERTY IMPORTED_LOCATION "${JANSSON_SRC_LIB}")
else()
  set_property(TARGET jansson PROPERTY IMPORTED_LOCATION "${iRODS_PREFIX}/lib/irods/externals/libjansson.a")
endif()



set(iRODS_LIBS
  dl
  pthread
  irods_client_api
  irods_client
  boost_filesystem
  boost_regex
  boost_system
  boost_thread
  boost_chrono
  boost_date_time
  boost_iostreams
  boost_program_options
  jansson)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

  # needed for linkage in OSX with iRODS 4.1.8+
  add_library(client_exec STATIC IMPORTED)
  set_property(TARGET client_exec PROPERTY IMPORTED_LOCATION ${iRODS_SRC_DIR}/iRODS/lib/client_exec/obj/irods_client_rule_execution_manager_factory.o)
  list(PREPEND iRODS_LIBS client_exec)

endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(iRODS_FOUND)
