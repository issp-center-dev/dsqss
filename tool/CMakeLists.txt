file(COPY dsqss DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(dla_scripts
    dla_latgen
    dla_hamgen
    dla_pgen
    dla_wvgen
    dla_alg
    dla_pre
    pmwa_pre)
foreach(script_name ${dla_scripts})
  configure_file(cmake/${script_name}.in
                 ${CMAKE_CURRENT_BINARY_DIR}/${script_name} @ONLY)
endforeach(script_name ${dla_scripts})

configure_file(cmake/install.sh.in ${CMAKE_CURRENT_BINARY_DIR}/install.sh @ONLY)
install(
  CODE "execute_process(COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/install.sh)")
