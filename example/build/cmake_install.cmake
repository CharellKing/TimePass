# Install script for directory: /home/ck/TimePass/example

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local/TimePass")
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
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/ck/TimePass/example/build/daytimetcpcli/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/daytimetcpsrv/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/echocli/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/echosvr/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/filelock/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/mutex_consume/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/mutex_multiconsume/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/rwlock/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/rwmutex/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/rwsem/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/sem_consume/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/sem_multibuff/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/sem_multiconsume/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_array/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_doublylist/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_hashlist/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_list/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_map/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_multihashmap/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_multihashset/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_multimap/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_queue/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_set/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/shm_tuplelist/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/st_echosvr/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/tcpudpsz/cmake_install.cmake")
  INCLUDE("/home/ck/TimePass/example/build/timer/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/ck/TimePass/example/build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/ck/TimePass/example/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
