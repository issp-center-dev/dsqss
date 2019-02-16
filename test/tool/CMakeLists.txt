file(COPY ${CMAKE_SOURCE_DIR}/tool/ DESTINATION ${CMAKE_BINARY_DIR}/test/tool FILES_MATCHING PATTERN "*.py")
file(COPY ${CMAKE_SOURCE_DIR}/tool/dsqss DESTINATION ${CMAKE_BINARY_DIR}/test/tool)
function(add_python_test test)
    add_test(NAME python_${test} COMMAND ${PYTHON_EXECUTABLE} ${test}.py)
    set_tests_properties(python_${test} PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/test/tool")
endfunction(add_python_test)

file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.py")
foreach(testfile ${TEST_FILES})
    configure_file(${testfile} ${CMAKE_BINARY_DIR}/test/tool COPYONLY)
endforeach()

if(MPI_FOUND)
set(python_test_src
  #   test_dsqss_pre_dla
  test_dsqss_pre_pmwa
)
else(MPI_FOUND)
set(python_test_src
  #   test_dsqss_pre_dla
)
endif(MPI_FOUND)
foreach(test ${python_test_src})
    add_python_test(${test})
endforeach(test)

add_subdirectory(data)
