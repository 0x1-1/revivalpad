# Make appdata optional when installing an application.
if(APPDATA)
    add_custom_target(appdata)

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/PO_files/")

    # Resolve the RevivalPad identity (application id, URLs, descriptions) into
    # an intermediate template, which itstool then merges the translations into.
    configure_file(
        "${PROJECT_SOURCE_DIR}/other/appdata/io.github._0x1_1.revivalpad.appdata.xml.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${REVIVALPAD_APP_ID}.appdata.xml.in"
        @ONLY)

    add_custom_command(TARGET appdata PRE_BUILD
            # Add an entry per language
            COMMAND msgfmt "${PROJECT_SOURCE_DIR}/other/appdata/PO_files/en.po" -o "${CMAKE_CURRENT_BINARY_DIR}/PO_files/en.mo"

            COMMAND itstool -i "${PROJECT_SOURCE_DIR}/other/appdata/appdata.its" -j "${CMAKE_CURRENT_BINARY_DIR}/${REVIVALPAD_APP_ID}.appdata.xml.in" -o "${REVIVALPAD_APP_ID}.appdata.xml" "${CMAKE_CURRENT_BINARY_DIR}/PO_files/*.mo"
            )

    # Only install an appdata file ifthe user requested to have one built.
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${REVIVALPAD_APP_ID}.appdata.xml" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo")
endif(APPDATA)
