configure_file(dsqssvars.sh.in dsqssvars-${DSQSS_VERSION}.sh @ONLY)

install(FILES dsqss_pre.py DESTINATION bin
        PERMISSIONS 
        OWNER_READ OWNER_WRITE OWNER_EXECUTE 
        GROUP_READ GROUP_EXECUTE 
        WORLD_READ WORLD_EXECUTE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsqssvars-${DSQSS_VERSION}.sh
        DESTINATION share/dsqss
        )
