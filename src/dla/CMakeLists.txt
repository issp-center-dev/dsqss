# include guard
cmake_minimum_required(VERSION 2.8)
if(${CMAKE_PROJECT_NAME} STREQUAL "Project")
  message(FATAL_ERROR "cmake should be executed not for 'src' subdirectory, but for the top directory of DLA.")
endif(${CMAKE_PROJECT_NAME} STREQUAL "Project")

include_directories(.)

set(dla_SOURCES random.cc dla.cc) 

add_executable(dla_B ${dla_SOURCES})
add_executable(dla_H ${dla_SOURCES})
install(TARGETS dla_B RUNTIME DESTINATION bin)
install(TARGETS dla_H RUNTIME DESTINATION bin)
target_compile_definitions(dla_B PRIVATE MEASURE_BOSON=1)
target_compile_definitions(dla_H PRIVATE MEASURE_SPIN=1)

if(MPI_FOUND)
  add_definitions(-DMULTI)
  target_include_directories(dla_H PUBLIC ${MPI_CXX_INCLUDE_DIRS})
  target_include_directories(dla_B PUBLIC ${MPI_CXX_INCLUDE_DIRS})
  target_link_libraries(dla_H ${MPI_CXX_LIBRARIES})
  target_link_libraries(dla_B ${MPI_CXX_LIBRARIES})
endif(MPI_FOUND)

add_subdirectory(generators)

add_custom_target(dla
  DEPENDS
  dla_H dla_B dla_alg lattgene_C lattgene_T hamgen_H hamgen_B cfgene sfgene
  )
