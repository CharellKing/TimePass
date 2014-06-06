# Install script for directory: /home/ck/git/TimePass-For-Eclipse/src/model

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "DEBUG")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/model" TYPE FILE FILES
    "/home/ck/git/TimePass-For-Eclipse/src/model/./mutex_multibuff_produce_consume.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./mutex_multibuff_thread_func_loader.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./sem_thread_func_loader.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./sem_multibuff_thread_func_loader.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./sem_multibuff_produce_consume.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./func_loader.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./mutex_produce_consume.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./mutex_thread_func_loader.h"
    "/home/ck/git/TimePass-For-Eclipse/src/model/./sem_produce_consume.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

