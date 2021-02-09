set(pythonpath_prefix "${CMAKE_CURRENT_BINARY_DIR}/pythonmodule")
set(pythonpath_build
    "${pythonpath_prefix}/lib/python${python_version_mm}/site-packages")
set(ENV{PYTHONPATH} "${pythonpath_build}:$ENV{PYTHONPATH}")
set(ENV{PATH} "${pythonpath_prefix}/bin:$ENV{PATH}")

include(${PROJECT_SOURCE_DIR}/config/FindPythonModule.cmake)
find_python_module(pipx)
if(NOT PIPX_FOUND)
  message(STATUS "pipx is not installed. install pipx into build directory")
  find_python_module(pip)
  if(NOT PIP_FOUND)
    message(
      FATAL_ERROR
        "pip seems not available. Please install pip by ${PYTHON_EXECUTABLE} -m ensurepip"
    )
  endif()
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install
                          --prefix=${pythonpath_prefix} pipx)
endif()

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

set(DSQSS_PYTHONPATH
    "${CMAKE_INSTALL_PREFIX}/pipx/venvs/dsqss/lib/python${python_version_mm}/site-packages")
configure_file(cmake/dsqssvars.sh.in
               ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh @ONLY)
configure_file(cmake/install.sh.in ${CMAKE_CURRENT_BINARY_DIR}/install.sh @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh
        DESTINATION share/dsqss)
install(
  CODE "execute_process(COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/install.sh)")
