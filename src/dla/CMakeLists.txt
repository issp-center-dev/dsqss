# include guard
if(${CMAKE_PROJECT_NAME} STREQUAL "Project")
  message(FATAL_ERROR "cmake should be executed not for 'src' subdirectory, but for the top directory of DLA.")
endif(${CMAKE_PROJECT_NAME} STREQUAL "Project")

include_directories(.)

set(dla_SOURCES random.cc dla.cc) 

add_executable(dla ${dla_SOURCES})
install(TARGETS dla RUNTIME DESTINATION bin)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  target_link_libraries(dla rt)
endif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

if(MPI_FOUND)
  add_definitions(-DMULTI)
  target_include_directories(dla PUBLIC ${MPI_CXX_INCLUDE_DIRS})
  target_link_libraries(dla ${MPI_CXX_LIBRARIES})
endif(MPI_FOUND)

set(dla_deps dla)

if(BUILD_OLD_GENERATORS)
add_subdirectory(generators)
set(dla_deps ${dla_deps} dla_alg_old lattgene_C lattgene_T hamgen_H hamgen_B cfgene sfgene)
endif(BUILD_OLD_GENERATORS)
