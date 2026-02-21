# CPack helper for this project
# Provides a simple function to configure CPack and install the runtime output

function(setup_cpack_for_target target output_dir)
    include(InstallRequiredSystemLibraries)

    # Basic package metadata; callers can override these variables before calling
    set(CPACK_PACKAGE_NAME "${target}")
    set(CPACK_PACKAGE_VERSION "1.0.0")
    set(CPACK_PACKAGE_CONTACT "")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

    # Install the built executable into the package root
    install(TARGETS ${target} RUNTIME DESTINATION ".")

    # Install everything from the output directory into the package root
    install(DIRECTORY "${output_dir}/" DESTINATION "." FILES_MATCHING PATTERN "*")

    include(CPack)
endfunction()
