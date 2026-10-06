cmake_minimum_required(VERSION 3.2...3.20.2 FATAL_ERROR)
project(DSQSS NONE)

# The version number of DSQSS. This is the only place to set it;
# the programs, the python package, and the manual take it from here.
set(DSQSS_VERSION 2.1.0)

include(${PROJECT_SOURCE_DIR}/cmake/DsqssVersion.cmake)
dsqss_normalize_version(DSQSS_VERSION)
find_package(Git QUIET)
dsqss_get_git_hash(${PROJECT_SOURCE_DIR} DSQSS_GIT_HASH)
message(STATUS "DSQSS version: v${DSQSS_VERSION} (${DSQSS_GIT_HASH})")

message(STATUS "CMake version: " ${CMAKE_VERSION})

# AddressSanitizer needs the frame pointer kept for usable stack traces
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fsanitize=address")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

enable_language(C CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall)
endif()

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


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/third-party/plog)

# dsqss_version.h is generated at the configuration, and again at every build
# to follow the commit hash of the repository
set(DSQSS_GENERATED_INCLUDE_DIR ${CMAKE_BINARY_DIR}/generated)
dsqss_generate_version_file(
  ${PROJECT_SOURCE_DIR} ${DSQSS_VERSION}
  ${PROJECT_SOURCE_DIR}/src/common/dsqss_version.h.in
  ${DSQSS_GENERATED_INCLUDE_DIR}/dsqss_version.h)
dsqss_version_file_command(
  dsqss_version_header_command
  ${PROJECT_SOURCE_DIR}/src/common/dsqss_version.h.in
  ${DSQSS_GENERATED_INCLUDE_DIR}/dsqss_version.h)
# BYPRODUCTS tells Ninja that the header may change during the build, so that
# the sources including it are compiled again in the same build
add_custom_target(dsqss_version_header
  COMMAND ${dsqss_version_header_command}
  BYPRODUCTS ${DSQSS_GENERATED_INCLUDE_DIR}/dsqss_version.h
  COMMENT "Updating the version information"
  VERBATIM)
include_directories(${DSQSS_GENERATED_INCLUDE_DIR})

set(python_version_required 3.8)

if(NOT PYTHON_EXECUTABLE)
  if(${CMAKE_VERSION} VERSION_LESS 3.12)
    find_package(PythonInterp ${python_version_required} REQUIRED)
    set(python_version_mm "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
  else(${CMAKE_VERSION} VERSION_LESS 3.12)
    find_package(Python3 ${python_version_required} COMPONENTS Interpreter REQUIRED)
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    set(python_version_mm "${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")
  endif(${CMAKE_VERSION} VERSION_LESS 3.12)
else()
  # check python version
  execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-V" OUTPUT_VARIABLE result ERROR_VARIABLE result)
  string(REGEX MATCH "^Python[ ]+[0-9]+\\.[0-9]+\\.[0-9]+" result "${result}")
  if(NOT result)
    message(FATAL_ERROR "${PYTHON_EXECUTABLE} seems not python executable")
  endif()
  message(STATUS "Given Python interpreter: ${PYTHON_EXECUTABLE}")
  string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" result "${result}")
  message(STATUS "Python version: ${result}")
  if("${result}" VERSION_LESS ${python_version_required})
    message(FATAL_ERROR "Python ${result} is not supported (${python_version_required} or later is required)")
  endif()
  string(REPLACE "." ";" result "${result}")
  set(result "${result}")
  list(GET result 0 Python3_VERSION_MAJOR)
  list(GET result 1 Python3_VERSION_MINOR)
  list(GET result 2 Python3_VERSION_MICRO)
  set(python_version_mm "${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")
endif()

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
