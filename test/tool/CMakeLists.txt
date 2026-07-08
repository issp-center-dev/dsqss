file(COPY ${CMAKE_BINARY_DIR}/tool/pmwa_pre DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# configure_file(${CMAKE_SOURCE_DIR}/tool/cmake/pmwa_pre.in ${CMAKE_CURRENT_BINARY_DIR}/pmwa_pre @ONLY)
set(BUILD_TOOL_DIR ${CMAKE_BINARY_DIR}/tool)
function(add_python_test test)
    add_test(NAME python_${test} COMMAND ${PYTHON_EXECUTABLE} -m pytest ${test}.py -v --tb=short)
    set_tests_properties(python_${test} PROPERTIES ENVIRONMENT "PYTHONPATH=${BUILD_TOOL_DIR}")
endfunction(add_python_test)

file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.py")
foreach(testfile ${TEST_FILES})
    configure_file(${testfile} ${CMAKE_BINARY_DIR}/test/tool COPYONLY)
endforeach()

set(python_unit_test_src
  test_util
  test_read_keyvalues
  test_prob_kernel
  test_parameter
  test_result
  test_lattice_factory
  test_lattice
  test_hamiltonian
  test_displacement
  test_wavevector
  test_algorithm
)

if(MPI_FOUND)
set(python_test_src
  #   test_dsqss_pre_dla
  test_pmwa_pre
)
else(MPI_FOUND)
set(python_test_src
  #   test_dsqss_pre_dla
)
endif(MPI_FOUND)

list(APPEND python_test_src ${python_unit_test_src})
foreach(test ${python_test_src})
    add_python_test(${test})
endforeach(test)

add_subdirectory(data)
