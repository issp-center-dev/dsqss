if (MPI_FOUND)
  set(testdirs dla pmwa tool)
else(MPI_FOUND)
  set(testdirs dla tool)
endif(MPI_FOUND)

foreach(testdir ${testdirs})
  add_subdirectory(${testdir})
endforeach(testdir)

add_test(NAME version/generate_version_file
  COMMAND ${CMAKE_COMMAND}
    -DDSQSS_CMAKE_DIR=${PROJECT_SOURCE_DIR}/cmake
    -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/version
    -P ${CMAKE_CURRENT_SOURCE_DIR}/version/test_generate_version_file.cmake)
