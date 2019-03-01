configure_file(common.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(test.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
file(GLOB jsons "*.json")
set(BUILD_TOOL_DIR ${CMAKE_BINARY_DIR}/tool)
foreach(json ${jsons})
  configure_file(${json} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
  file(RELATIVE_PATH jsonfilename ${CMAKE_CURRENT_SOURCE_DIR} ${json})

  add_test(NAME "dla/${jsonfilename}"
    COMMAND env PYTHONPATH=${BUILD_TOOL_DIR} ${PYTHON_EXECUTABLE} test.py ${CMAKE_BINARY_DIR}/src/dla ${CMAKE_CURRENT_BINARY_DIR}/${jsonfilename}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
    )
endforeach(json)

configure_file(checkpoint.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
add_test(NAME "dla/checkpoint"
  COMMAND env PYTHONPATH=${BUILD_TOOL_DIR} ${PYTHON_EXECUTABLE} checkpoint.py ${CMAKE_BINARY_DIR}/src/dla
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
  )
