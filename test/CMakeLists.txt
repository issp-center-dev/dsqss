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
    set(missing_modules ${module_name} ${missing_modules})
  endif(NOT ${uppername}_FOUND)
endforeach(module_name numpy scipy toml)

list(LENGTH missing_modules num_missing_modules)
if(num_missing_modules)
  set(pythonpath_testing "${CMAKE_CURRENT_BINARY_DIR}/pythonmodule")
  find_python_module(pip)
  if(NOT PIP_FOUND)
    file(DOWNLOAD https://bootstrap.pypa.io/get-pip.py "${CMAKE_CURRENT_BINARY_DIR}/get-pip.py")
    execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/get-pip.py --prefix=${pythonpath_testing})
    execute_process(COMMAND PYTHONPATH=${pythonpath_testing} ${PYTHON_EXECUTABLE} -m pip install -U pip)
  endif(NOT PIP_FOUND)
  foreach(module_name ${missing_modules})
    execute_process(
      COMMAND PYTHONPATH=${pythonpath_testing} ${PYTHON_EXECUTABLE} -m pip install --prefix=${pythonpath_testing} ${module_name}
      )
  endforeach(module_name ${missing_modules})
endif(num_missing_modules)


foreach(testdir ${testdirs})
  add_subdirectory(${testdir})
endforeach(testdir)
