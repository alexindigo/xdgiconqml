cmake_minimum_required(VERSION 3.16)
project(xdgiconqml
    VERSION 0.0.0
    DESCRIPTION "XDG Icon Theme QML Plugin"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Qml Quick)
qt_policy(SET QTP0001 NEW)

include(GNUInstallDirs)

option(WITH_DBUS_BROADCAST
    "Enable D-Bus theme-changed broadcast listener. Adds Qt6::DBus dependency."
    OFF
)

set(XDGICONQML_SOURCES
    xdgicon.cpp
    xdgicon.h
    xdgicontheme.cpp
    xdgicontheme.h
    xdgindexparse.cpp
    xdgindexparse.h
    xdglookup.cpp
    xdglookup.h
    xdgpathwatcher.cpp
    xdgpathwatcher.h
    xdgthemewatcher.cpp
    xdgthemewatcher.h
    xdgcache.cpp
    xdgcache.h
    xdgbroadcast.cpp
    xdgbroadcast.h
    xdgtypes.h
)

qt_add_qml_module(xdgiconqml
    URI XdgIcon
    VERSION 1.0
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/XdgIcon
    SOURCES
        ${XDGICONQML_SOURCES}
        xdgiconplugin.cpp
)

target_link_libraries(xdgiconqml PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
)

if(WITH_DBUS_BROADCAST)
    find_package(Qt6 6.8 REQUIRED COMPONENTS DBus)
    target_link_libraries(xdgiconqml PRIVATE Qt6::DBus)
    target_compile_definitions(xdgiconqml PRIVATE WITH_DBUS_BROADCAST)
endif()

# ==================== INSTALL ====================

install(TARGETS xdgiconqml
    EXPORT xdgiconqmlTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/XdgIcon/
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/XdgIcon
)

install(FILES
    xdgicon.h
    xdgicontheme.h
    xdgindexparse.h
    xdglookup.h
    xdgpathwatcher.h
    xdgthemewatcher.h
    xdgcache.h
    xdgbroadcast.h
    xdgtypes.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xdgiconqml
)

# ==================== CMAKE PACKAGE CONFIG ====================

include(CMakePackageConfigHelpers)

set(XDGICONQML_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/xdgiconqml)

install(EXPORT xdgiconqmlTargets
    FILE xdgiconqmlTargets.cmake
    NAMESPACE xdgiconqml::
    DESTINATION ${XDGICONQML_CMAKE_INSTALL_DIR}
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/xdgiconqmlConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/xdgiconqmlConfig.cmake
    INSTALL_DESTINATION ${XDGICONQML_CMAKE_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/xdgiconqmlConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/xdgiconqmlConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/xdgiconqmlConfigVersion.cmake
    DESTINATION ${XDGICONQML_CMAKE_INSTALL_DIR}
)

# ==================== TESTS ====================

option(BUILD_TEST_MODE "Build C++ test executables" ON)

if(BUILD_TEST_MODE)
    find_package(Qt6 6.8 REQUIRED COMPONENTS Test)

    macro(add_xdgiconqml_test NAME)
        add_executable(${NAME} tests/${NAME}.cpp)
        target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(${NAME} PRIVATE
            Qt6::Core Qt6::Qml Qt6::Quick Qt6::Test xdgiconqml
        )
        enable_testing()
        add_test(NAME ${NAME} COMMAND ${NAME})
        set_tests_properties(${NAME} PROPERTIES
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        )
        if(ENABLE_COVERAGE)
            target_compile_options(${NAME} PRIVATE --coverage -fprofile-abs-path)
            target_link_options(${NAME} PRIVATE --coverage -fprofile-abs-path)
        endif()
    endmacro()

    add_xdgiconqml_test(test_xdgindexparse)
    add_xdgiconqml_test(test_xdglookup)
endif()

# ==================== COVERAGE ====================

option(ENABLE_COVERAGE "Instrument targets for code coverage collection" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Enabling code coverage flags")

    target_compile_options(xdgiconqml PRIVATE --coverage -fprofile-abs-path)
    target_link_options(xdgiconqml PRIVATE --coverage -fprofile-abs-path)
endif()
