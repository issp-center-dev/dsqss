cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(DSQSS NONE)
set(DSQSS_VERSION 1.2.0)

if(CONFIG)    
  message(STATUS "Loading configration: " ${PROJECT_SOURCE_DIR}/config/${CONFIG}.cmake)
  include(${PROJECT_SOURCE_DIR}/config/${CONFIG}.cmake)
endif(CONFIG)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build" FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_MPI "Enable MPI Parallelization" ON)
option(Testing "Enable testing" ON)
option(Document "Build HTML document" OFF)

option(USE_SYSTEM_BOOST "use Boost installed in system" OFF)

enable_language(C CXX)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MACOSX_RPATH 1)

option(DEBUG "Debug" OFF)

find_package(LAPACK)

if(ENABLE_MPI)
  find_package(MPI)
endif(ENABLE_MPI)

if(USE_SYSTEM_BOOST)
  find_package(Boost REQUIRED)
else(USE_SYSTEM_BOOST)
  set(Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/third-party/boost)
  message(STATUS "Use bundled Boost headers (1.67)")
endif(USE_SYSTEM_BOOST)
message(STATUS "Boost headers: ${Boost_INCLUDE_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src/dla)
if(MPI_FOUND)
  add_subdirectory(src/pmwa)
endif(MPI_FOUND)
add_subdirectory(tool)

if (Testing)
  enable_testing()
  add_subdirectory(test)
endif()

if (Document)
  add_subdirectory(doc)
endif(Document)

add_subdirectory(sample)
