# Golden-file regression tests for the legacy generators.
# Only registered when the generators are actually built.
if(BUILD_OLD_GENERATORS)
  add_test(
    NAME generators_golden
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/run_generator_tests.py
            --bindir $<TARGET_FILE_DIR:dla_alg_old>
            --datadir ${CMAKE_CURRENT_SOURCE_DIR}/data
  )
endif(BUILD_OLD_GENERATORS)
