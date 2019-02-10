file(COPY dsqss DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file(dist/dsqss-2.0.0-py2.py3-none-any.whl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(dsqssvars.sh.in dsqssvars-${DSQSS_VERSION}.sh @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh
        DESTINATION share/dsqss
        )

set(DSQSS_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/lib/python${python_version_mm}/site-packages")

if(BUILD_NEW_GENERATORS)
  configure_file(install_wheel.sh.in install_wheel.sh @ONLY)
  install(CODE "execute_process(COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/install_wheel.sh)")
endif(BUILD_NEW_GENERATORS)
