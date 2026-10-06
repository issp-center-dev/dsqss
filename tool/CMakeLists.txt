# The python package is staged in the build directory together with
# pyproject.toml and _version.py, which have the version information,
# and is installed from there.
set(DSQSS_TOOL_STAGE_DIR ${CMAKE_CURRENT_BINARY_DIR})

file(COPY dsqss DESTINATION ${DSQSS_TOOL_STAGE_DIR})
configure_file(pyproject.toml.in ${DSQSS_TOOL_STAGE_DIR}/pyproject.toml @ONLY)
dsqss_generate_version_file(
  ${PROJECT_SOURCE_DIR} ${DSQSS_VERSION}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/_version.py.in
  ${DSQSS_TOOL_STAGE_DIR}/dsqss/_version.py)

# updated at every build to follow the source tree and the commit hash
dsqss_version_file_command(
  dsqss_version_py_command
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/_version.py.in
  ${DSQSS_TOOL_STAGE_DIR}/dsqss/_version.py)
add_custom_target(dsqss_tool ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/dsqss ${DSQSS_TOOL_STAGE_DIR}/dsqss
  COMMAND ${dsqss_version_py_command}
  COMMENT "Updating the python package"
  VERBATIM)

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
