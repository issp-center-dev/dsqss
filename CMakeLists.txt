cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(DSQSS NONE)
set(DSQSS_VERSION 2.1-dev)

message(STATUS "CMake version: " ${CMAKE_VERSION})

set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address -fomit-frame-pointer")

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

option(BUILD_NEW_GENERATORS "build new file-generators" ON)
option(BUILD_OLD_GENERATORS "build old file-generators" OFF)

enable_language(C CXX)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MACOSX_RPATH 1)

option(DEBUG "Debug" OFF)

if(DEBUG)
  add_definitions(-DDEB)
endif(DEBUG)

if(ENABLE_MPI)
  find_package(MPI)
  if(MPI_FOUND)
    if(NOT MPI_CXX_INCLUDE_DIRS)
      if(MPI_CXX_INCLUDE_PATH)
        set(MPI_CXX_INCLUDE_DIRS ${MPI_CXX_INCLUDE_PATH})
      else(MPI_CXX_INCLUDE_PATH)
        set(MPI_CXX_INCLUDE_DIRS ${MPI_INCLUDE_PATH})
      endif(MPI_CXX_INCLUDE_PATH)
    endif(NOT MPI_CXX_INCLUDE_DIRS)
    if(NOT MPI_CXX_LIBRARIES)
      set(MPI_CXX_LIBRARIES ${MPI_LIBRARIES})
    endif(NOT MPI_CXX_LIBRARIES)
  endif(MPI_FOUND)
endif(ENABLE_MPI)

if(USE_SYSTEM_BOOST)
  find_package(Boost REQUIRED)
else(USE_SYSTEM_BOOST)
  set(Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/third-party/boost)
  message(STATUS "Use bundled Boost headers (1.67)")
endif(USE_SYSTEM_BOOST)
message(STATUS "Boost headers: ${Boost_INCLUDE_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/third-party/plog)

if(BUILD_NEW_GENERATORS OR Testing OR Document)
  if(${CMAKE_VERSION} VERSION_LESS 3.12)
    find_package(PythonInterp 3.6 REQUIRED)
    set(python_version_mm "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
  else(${CMAKE_VERSION} VERSION_LESS 3.12)
    find_package(Python3 3.6 COMPONENTS Interpreter REQUIRED)
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    set(python_version_mm "${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")
  endif(${CMAKE_VERSION} VERSION_LESS 3.12)

endif()

add_subdirectory(src/dla)
if(MPI_FOUND)
  add_subdirectory(src/pmwa)
endif(MPI_FOUND)

if(BUILD_NEW_GENERATORS)
  add_subdirectory(tool)
endif()

if (Testing)
  enable_testing()
  add_subdirectory(test)
endif()

if (Document)
  add_subdirectory(doc)
endif(Document)

add_subdirectory(sample)
