# include guard
if (${CMAKE_PROJECT_NAME} STREQUAL "Project")
    message(FATAL_ERROR "cmake should be executed not for 'src' subdirectory, but for the top directory of DLA.")
endif (${CMAKE_PROJECT_NAME} STREQUAL "Project")

add_subdirectory(./lib)
include_directories(./inc)
include_directories(./lib)

set(pmwa_SOURCES PMWA.cpp configuration.cpp graphspace.cpp lattice.cpp)
set(pmwa_SOURCES_H probability.H.cpp quantities.H.cpp)
set(pmwa_SOURCES_B probability.B.cpp quantities.B.cpp)

set(lattgene_P_SOURCES lattgene.cc)

add_executable(lattgene_P ${lattgene_P_SOURCES})
install(TARGETS lattgene_P RUNTIME DESTINATION bin)

add_executable(pmwa_H ${pmwa_SOURCES} ${pmwa_SOURCES_H})
add_executable(pmwa_B ${pmwa_SOURCES} ${pmwa_SOURCES_B})

install(TARGETS pmwa_H RUNTIME DESTINATION bin)
install(TARGETS pmwa_B RUNTIME DESTINATION bin)
target_compile_definitions(pmwa_H PRIVATE MEASURE_SPIN=1)
target_compile_definitions(pmwa_B PRIVATE MEASURE_BOSON=1)

target_link_libraries(pmwa_H ${LAPACK_LIBRARIES} RDM STDMA)
target_link_libraries(pmwa_B ${LAPACK_LIBRARIES} RDM STDMA)
target_include_directories(pmwa_H PUBLIC ${MPI_CXX_INCLUDE_DIRS})
target_include_directories(pmwa_B PUBLIC ${MPI_CXX_INCLUDE_DIRS})
target_link_libraries(pmwa_H ${MPI_CXX_LIBRARIES})
target_link_libraries(pmwa_B ${MPI_CXX_LIBRARIES})

add_custom_target(pmwa
  DEPENDS
  pmwa_H pmwa_B lattgene_P
  )
