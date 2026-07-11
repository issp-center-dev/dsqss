if (MPI_FOUND)
  set(testdirs dla pmwa tool generators)
else(MPI_FOUND)
  set(testdirs dla tool generators)
endif(MPI_FOUND)

foreach(testdir ${testdirs})
  add_subdirectory(${testdir})
endforeach(testdir)
