if (MPI_FOUND)
  set(testdirs dla pmwa tool)
else(MPI_FOUND)
  set(testdirs dla tool)
endif(MPI_FOUND)

foreach(testdir ${testdirs})
  add_subdirectory(${testdir})
endforeach(testdir)
