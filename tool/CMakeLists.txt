file(COPY dsqss DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(DSQSS_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/lib/python${python_version_mm}/site-packages")

set(dla_scripts dla_latgen dla_hamgen dla_sfgen dla_alg dla_pre)
foreach(script_name ${dla_scripts})
  configure_file(cmake/${script_name}.in ${CMAKE_CURRENT_BINARY_DIR}/${script_name} @ONLY)
endforeach(script_name ${dla_scripts})

configure_file(dist/dsqss-2.0.0-py2.py3-none-any.whl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(cmake/dsqssvars.sh.in ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh
        DESTINATION share/dsqss
        )

if(BUILD_NEW_GENERATORS)
  configure_file(cmake/install_wheel.sh.in ${CMAKE_CURRENT_BINARY_DIR}/install_wheel.sh @ONLY)
  install(CODE "execute_process(COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/install_wheel.sh)")
endif(BUILD_NEW_GENERATORS)
