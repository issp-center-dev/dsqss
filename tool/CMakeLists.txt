file(COPY setup.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY dsqss DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(dla_scripts dla_latgen dla_hamgen dla_sfgen dla_alg dla_pre)
foreach(script_name ${dla_scripts})
  configure_file(cmake/${script_name}.in ${CMAKE_CURRENT_BINARY_DIR}/${script_name} @ONLY)
endforeach(script_name ${dla_scripts})

set(DSQSS_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/lib/python${python_version_mm}/site-packages")
configure_file(cmake/dsqssvars.sh.in ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh @ONLY)
configure_file(cmake/install.sh.in ${CMAKE_CURRENT_BINARY_DIR}/install.sh @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh
        DESTINATION share/dsqss
        )
install(CODE "execute_process(COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/install.sh)")
