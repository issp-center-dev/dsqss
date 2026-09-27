configure_file(common.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(test.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
file(GLOB jsons "*.json")
set(BUILD_TOOL_DIR ${CMAKE_BINARY_DIR}/tool)
set(PYTHONPATH "${BUILD_TOOL_DIR}:${pythonpath_build}")
foreach(json ${jsons})
  configure_file(${json} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
  file(RELATIVE_PATH jsonfilename ${CMAKE_CURRENT_SOURCE_DIR} ${json})

  add_test(NAME "dla/${jsonfilename}"
    COMMAND env PYTHONPATH=${PYTHONPATH} ${PYTHON_EXECUTABLE} test.py ${CMAKE_BINARY_DIR}/src/dla ${CMAKE_CURRENT_BINARY_DIR}/${jsonfilename}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
    )
endforeach(json)

configure_file(checkpoint.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
add_test(NAME "dla/checkpoint"
  COMMAND env PYTHONPATH=${PYTHONPATH} ${PYTHON_EXECUTABLE} checkpoint.py ${CMAKE_BINARY_DIR}/src/dla
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
  )

# "v2.1.0 (7b79e710)" or "v2.1.0 (7b79e710-dirty)";
# the hash is of the commit at the build
set(hex "[0-9a-f]")
set(version_pattern
  "^v${DSQSS_VERSION} \\((${hex}${hex}${hex}${hex}${hex}${hex}${hex}${hex}(-dirty)?|unknown)\\)\n$")
add_test(NAME "dla/version" COMMAND dla --version)
set_tests_properties("dla/version" PROPERTIES
  PASS_REGULAR_EXPRESSION "${version_pattern}")
