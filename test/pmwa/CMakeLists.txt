function(add_python_test test)
    add_test(NAME python_${test} COMMAND ${PYTHON_EXECUTABLE} ${test}.py)
    set_tests_properties(python_${test} PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/test/tool:${pythonpath_build}")
endfunction(add_python_test)

configure_file(test.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
file(GLOB jsons "*.json")
foreach(json ${jsons})
  configure_file(${json} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
  file(RELATIVE_PATH jsonfilename ${CMAKE_CURRENT_SOURCE_DIR} ${json})

  add_test(NAME "pmwa/${jsonfilename}"
    COMMAND env PYTHONPATH=${pythonpath_build} ${PYTHON_EXECUTABLE} test.py ${CMAKE_BINARY_DIR}/src/pmwa ${CMAKE_CURRENT_BINARY_DIR}/${jsonfilename}
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

# "v2.1.0 (7b79e710)" or "v2.1.0 (7b79e710-dirty)";
# the hash is of the commit at the build
set(hex "[0-9a-f]")
set(version_pattern
  "^v${DSQSS_VERSION} \\((${hex}${hex}${hex}${hex}${hex}${hex}${hex}${hex}(-dirty)?|unknown)\\)\n$")
foreach(target pmwa_H pmwa_B lattgene_P)
  add_test(NAME "pmwa/version_${target}" COMMAND ${target} --version)
  set_tests_properties("pmwa/version_${target}" PROPERTIES
    PASS_REGULAR_EXPRESSION "${version_pattern}")
endforeach()

add_subdirectory(data)
