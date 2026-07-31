# include guard
if(${CMAKE_PROJECT_NAME} STREQUAL "Project")
  message(FATAL_ERROR "cmake should be executed not for 'src' subdirectory, but for the top directory of DLA.")
endif(${CMAKE_PROJECT_NAME} STREQUAL "Project")

include_directories(.)
set(RDM_SOURCES My_rdm.hrd.cpp)
set(STDMA_SOURCES stdma.cpp)

add_library(RDM STATIC ${RDM_SOURCES})
add_library(STDMA STATIC ${STDMA_SOURCES})

