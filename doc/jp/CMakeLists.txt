set(SPHINX_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/_build)
set(SPHINX_CACHE_DIR ${CMAKE_CURRENT_BINARY_DIR}/_doctrees)
set(SPHINX_HTML_DIR ${CMAKE_CURRENT_BINARY_DIR}/html)
set(SPHINX_PDF_DIR ${CMAKE_CURRENT_BINARY_DIR}/pdf)
set(DSQSS_DOC_DIR_JP share/dsqss/${DSQSS_VERSION}/doc/jp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py ${CMAKE_CURRENT_BINARY_DIR})

set(cmd_list
  env PYTHONPATH="${pythonpath_build}:$ENV{PYTHONPATH}"
  ${SPHINX_EXECUTABLE}
  -b html
  -d ${SPHINX_CACHE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SPHINX_HTML_DIR}
)

if(ENCHANT_LIBRARIES)
  list(INSERT cmd_list 0 env PYENCHANT_LIBRARY_PATH=${ENCHANT_LIBRARIES})
endif()

add_custom_target(doc-jp-html ALL
  COMMAND
  ${cmd_list}
  )

if(LATEX_FOUND)

  set(cmd_list
      env PYTHONPATH="${pythonpath_build}:$ENV{PYTHONPATH}"
      ${ENC_ENV}
      ${SPHINX_EXECUTABLE}
      -b latex
      -d ${SPHINX_CACHE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${SPHINX_PDF_DIR}
      )
  if(ENCHANT_LIBRARIES)
    list(INSERT cmd_list 0 env PYENCHANT_LIBRARY_PATH=${ENCHANT_LIBRARIES})
  endif()

  add_custom_target(doc-jp-pdf ALL
    COMMAND
      ${cmd_list}
    COMMAND
      cd pdf && make
    )
  install(DIRECTORY ${SPHINX_PDF_DIR}
    DESTINATION ${DSQSS_DOC_DIR_JP}
    FILES_MATCHING PATTERN "*.pdf"
    )

  add_custom_target(doc-jp DEPENDS doc-jp-html doc-jp-pdf)
else(LATEX_FOUND)
  add_custom_target(doc-jp DEPENDS doc-jp-html)
endif(LATEX_FOUND)

install(DIRECTORY ${SPHINX_HTML_DIR}
  DESTINATION ${DSQSS_DOC_DIR_JP}
  PATTERN "_sources" EXCLUDE
  PATTERN ".buildinfo" EXCLUDE
  PATTERN "objects.inv" EXCLUDE
  )
