if (MPI_FOUND)
  set(testdirs dla pmwa tool)
else(MPI_FOUND)
  set(testdirs dla tool)
endif(MPI_FOUND)

include(${PROJECT_SOURCE_DIR}/config/FindPythonModule.cmake)

# set(python_missing_modules "")
foreach(module_name numpy scipy toml)
  find_python_module(${module_name})
  string(TOUPPER ${module_name} uppername)
  if(NOT ${uppername}_FOUND)
    set(missing_modules ${missing_modules} ${module_name})
  endif(NOT ${uppername}_FOUND)
endforeach(module_name numpy scipy toml)

list(LENGTH missing_modules num_missing_modules)
if(num_missing_modules)
  set(pythonpath_prefix "${CMAKE_CURRENT_BINARY_DIR}/pythonmodule")
  set(pythonpath_build "${pythonpath_prefix}/lib/python${python_version_mm}/site-packages")
  find_python_module(pip)
  if(NOT PIP_FOUND)
    file(DOWNLOAD https://bootstrap.pypa.io/get-pip.py "${CMAKE_CURRENT_BINARY_DIR}/get-pip.py")
    execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/get-pip.py --prefix=${pythonpath_prefix})
    execute_process(COMMAND env PYTHONPATH=${pythonpath_build} ${PYTHON_EXECUTABLE} -m pip install -U pip)
  endif(NOT PIP_FOUND)
  foreach(module_name ${missing_modules})
    message(STATUS "env PYTHONPATH=${pythonpath_build} ${PYTHON_EXECUTABLE} -m pip install --prefix=${pythonpath_prefix} ${module_name}")
    execute_process(
      COMMAND env PYTHONPATH=${pythonpath_build} ${PYTHON_EXECUTABLE} -m pip install --prefix=${pythonpath_prefix} ${module_name}
      )
  endforeach(module_name ${missing_modules})
endif(num_missing_modules)


foreach(testdir ${testdirs})
  add_subdirectory(${testdir})
endforeach(testdir)
