# include guard
cmake_minimum_required(VERSION 2.8)
if(${CMAKE_PROJECT_NAME} STREQUAL "Project")
  message(FATAL_ERROR "cmake should be executed not for 'src' subdirectory, but for the top directory of DLA.")
endif(${CMAKE_PROJECT_NAME} STREQUAL "Project")

include_directories(.)

set(dla_alg_SOURCES dla_alg.cc)
set(cfgene_SOURCES cfgene.cc)
set(sfgene_SOURCES sfgene.cc)
set(lattgene_C_SOURCES lattgene_C.cc)
set(lattgene_T_SOURCES lattgene_T.cc)
set(hamgen_H_SOURCES exact_H.cc)
set(hamgen_B_SOURCES exact_B.cc)

set(exelist dla_alg cfgene sfgene lattgene_C lattgene_T hamgen_H hamgen_B)

foreach(exe IN LISTS exelist)
  add_executable(${exe} ${${exe}_SOURCES})
  install(TARGETS ${exe} RUNTIME DESTINATION bin)
endforeach(exe)

target_link_libraries(hamgen_H ${LAPACK_LIBRARIES})
target_link_libraries(hamgen_B ${LAPACK_LIBRARIES})
