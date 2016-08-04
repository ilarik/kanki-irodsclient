
# use auto moc feature to build qt meta objects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

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
add_executable(irodsclient MACOSX_BUNDLE ${CXX_FILES} ${UISrcs} ${QT_WRAP} ${UI_RESOURCES})

# include necessary Qt5 modules for linkage 
qt5_use_modules(irodsclient Core Gui Svg Xml)

target_link_libraries(
  irodsclient
  ${iRODS_LIBS}
  ssl
  crypto
  )

install(TARGETS irodsclient
  RUNTIME DESTINATION bin
  BUNDLE DESTINATION /Applications)

install(FILES icons/irodsclient.png
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
