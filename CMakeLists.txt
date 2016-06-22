project(kuseraccount)
cmake_minimum_required(VERSION 2.8.12)
set(PROJECT_VERSION "5.4.3")
set(PIC_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/kdm/pics/users)

add_definitions(-std=c++11 -Werror)
add_definitions("-DPROJECT_VERSION=\"${PROJECT_VERSION}\"")
add_definitions("-DPIC_DATA_DIR=\"${PIC_DATA_DIR}\"")

option(ENABLE_DEBUG "Enable debug" OFF)
if(ENABLE_DEBUG)
    add_definitions(-g -DDEBUG)
endif()

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(KF5 REQUIRED COMPONENTS KDELibs4Support Wallet)
find_package(QtAccountsService)                                                    
set_package_properties(QtAccountsService PROPERTIES
    DESCRIPTION "Qt-style API to use freedesktop.org's AccountsService DBus service"
    URL "http://git.isoft.zhcn.cc/zhaixiang/qtaccountsservice"
    TYPE REQUIRED
)
find_package(PolkitQt5-1 REQUIRED 0.103.0)
find_package(PkgConfig)
pkg_check_modules(PWQUALITY REQUIRED pwquality)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

add_subdirectory(kcm)
add_subdirectory(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
