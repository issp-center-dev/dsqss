function(add_python_test test)
    add_test(NAME python_${test} COMMAND ${PYTHON_EXECUTABLE} ${test}.py)
    set_tests_properties(python_${test} PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/test/tool")
endfunction(add_python_test)

configure_file(test.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
file(GLOB jsons "*.json")
foreach(json ${jsons})
  configure_file(${json} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
  file(RELATIVE_PATH jsonfilename ${CMAKE_CURRENT_SOURCE_DIR} ${json})

  add_test(NAME "pmwa/${jsonfilename}"
    COMMAND ${PYTHON_EXECUTABLE} test.py ${CMAKE_BINARY_DIR}/src/pmwa ${CMAKE_CURRENT_BINARY_DIR}/${jsonfilename}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
    )
endforeach(json)

file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.py")
foreach(testfile ${TEST_FILES})
    configure_file(${testfile} ${CMAKE_BINARY_DIR}/test/pmwa COPYONLY)
endforeach()

set(python_test_src
  test_pmwa_restart
)
foreach(test ${python_test_src})
    add_python_test(${test})
endforeach(test)

add_subdirectory(data)
