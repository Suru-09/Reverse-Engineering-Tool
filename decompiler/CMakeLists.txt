cmake_minimum_required(VERSION 3.22)

project(Reverse-Engineering-Tool)

set(CMAKE_VERBOSE_MAKEFILE ON)

# change from default CLion output directory to my preferred one
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

# ignore building STATIC instead of SHARED libs
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_REQUIRED 20)
SET(CMAKE_CXX_STANDARD 20)

SET(arch "arch" CACHE STRING "")

message(STATUS "Architecture compiled on: <${arch}>")

# LLVM DEPENDECIES ----- START --------
find_package(LLVM REQUIRED CONFIG)

message(STATUS "=====================================")
message(STATUS "Found LLVM: <${LLVM_PACKAGE_VERSION}>")
message(STATUS "Using LLVMConfig.cmake in: <${LLVM_DIR}>")
message(STATUS "=====================================")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS_LIST})
# LLVM DEPENDECIES ----- END --------

# CPPREST DEPENDECIES ----- START --------

find_package(cpprestsdk REQUIRED)

include_directories()

if(cpprestsdk_FOUND)
    message(STATUS "=====================================")
    message(STATUS "cpprestsdk_DIR          : ${cpprestsdk_DIR}")
    message(STATUS "cpprestsdk_VERSION      : ${cpprestsdk_VERSION}")
    message(STATUS "cpprestsdk_INCLUDE_DIR  : ${cpprestsdk_INCLUDE_DIR}")
    message(STATUS "=====================================")
    message(STATUS "OPENSSL_VERSION      : ${OPENSSL_VERSION}")
    message(STATUS "OPENSSL_INCLUDE_DIR  : ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OPENSSL_LIBRARIES    : ${OPENSSL_LIBRARIES}")
    message(STATUS "=====================================")
endif(cpprestsdk_FOUND)

# CURL DEPENDECIES ----- START --------

find_package(CURL REQUIRED)

if(CURL_FOUND)
    message(STATUS "CURL_INCLUDE_DIR  : ${CURL_INCLUDE_DIR}")
    message(STATUS "CURL_LIBRARIES    : ${CURL_LIBRARIES}")
    message(STATUS "=====================================")
endif(CURL_FOUND)

# CURL DEPENDECIES ----- END --------

# BOOST DEPENDECIES ----- START --------

IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
    message(STATUS "Boost_INCLUDE_DIRS  : ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARY_DIRS    : ${Boost_LIBRARY_DIRS}")
    message(STATUS "=====================================")
ENDIF(Boost_FOUND)
# BOOST DEPENDECIES ----- END --------

# CPPREST DEPENDECIES ----- END --------

# SPDLOG DEPENDECIES ----- START --------
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

if(${arch} STREQUAL "arm64")
    set(spdlog_INCLUDE_DIRS /opt/homebrew/Cellar/spdlog/1.11.0/include)
    set(fmt_INCLUDE_DIRS /opt/homebrew/Cellar/fmt/9.1.0/include)
    message(STATUS "SPDLOG_INCLUDE_DIRS  : ${spdlog_INCLUDE_DIRS}")
    message(STATUS "FMT_INCLUDE_DIRS     : ${fmt_INCLUDE_DIRS}")
else()
    message(STATUS "SPDLOG_INCLUDE_DIRS  : ${spdlog_INCLUDE_DIRS}")
    message(STATUS "FMT_INCLUDE_DIRS     : ${fmt_INCLUDE_DIRS}")
endif()   

# SPDLOG DEPENDECIES ----- END --------

# GOOGLE TEST DEPENDENCIES ----- START --------

find_package(GTest REQUIRED)
message(STATUS "=====================================")

# GOOGLE TEST DEPENDENCIES ----- END --------

# QT DEPENDENCIES ----- START --------
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(Qt6_FOUND)
    message(STATUS "=====================================")
    message(STATUS "Qt6_DIR          : ${Qt6_DIR}")
    message(STATUS "Qt6_VERSION      : ${Qt6_VERSION}")
    message(STATUS "Qt6Core_DIR      : ${Qt6Core_DIR}")
    message(STATUS "Qt6Core_VERSION  : ${Qt6Core_VERSION}")
    message(STATUS "Qt6Widgets_DIR   : ${Qt6Widgets_DIR}")
    message(STATUS "Qt6Widgets_VERSION: ${Qt6Widgets_VERSION}")
    message(STATUS "=====================================")



    #include_directories(${Qt6Core_INCLUDE_DIRS})
    #include_directories(${Qt6Widgets_INCLUDE_DIRS})
endif(Qt6_FOUND)

# QT DEPENDENCIES ----- END --------

# Include dirs for C++ files
set(INCL_DIR
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/testing_files
    ${Capstone_INCLUDE}
    ${LLVM_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
    ${cpprestsdk_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    $<$<STREQUAL:"${arch}","arm64">:${fmt_INCLUDE_DIRS}>
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6Core_INCLUDE_DIRS}
)

# compiling flags
set(CXX_FLAGS 
    -O3
    -Wall
)

add_subdirectory(src)
#add_subdirectory(test)