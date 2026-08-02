# This will ensure that the *.qm files will be stored
# in the right place within the build directory.
file(GLOB revivalpad_TRANSLATIONS revivalpad_*.ts)
list(APPEND revivalpad_TRANSLATIONS revivalpad.ts)
file(GLOB_RECURSE revivalpad_BASE_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE revivalpad_BASE_FORMS ${PROJECT_SOURCE_DIR}/src/*.ui)


if(QT_VERSION VERSION_GREATER_EQUAL 5.15)
    if(UPDATE_TRANSLATIONS)
        message("Update translations - lupdate ON")
        if(TRANS_REMOVE_OBSOLETE)
            message("getting rid of old text entries: \"lupdate -noobsolete\" ON")
            QT_CREATE_TRANSLATION(revivalpad_QMFILES ${revivalpad_BASE_SOURCES}
                    ${revivalpad_BASE_FORMS} ${revivalpad_TRANSLATIONS} OPTIONS -no-obsolete)
        else()
            QT_CREATE_TRANSLATION(revivalpad_QMFILES ${revivalpad_BASE_SOURCES}
                    ${revivalpad_BASE_FORMS} ${revivalpad_TRANSLATIONS})
        endif(TRANS_REMOVE_OBSOLETE)
    else()
        QT_ADD_TRANSLATION(revivalpad_QMFILES ${revivalpad_TRANSLATIONS})
    endif(UPDATE_TRANSLATIONS)
else()
    # Since 5.15 QT commands are universal
    QT5_ADD_TRANSLATION(revivalpad_QMFILES ${revivalpad_TRANSLATIONS})
    if(UPDATE_TRANSLATIONS)
        message(WARNING "Updating translations is only supported with Qt 5.15 or newer.")
    endif()
endif()


add_custom_target(updateqm DEPENDS ${revivalpad_QMFILES} COMMENT "Update translation files based on source and ui files")
install(FILES ${revivalpad_QMFILES}
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${REVIVALPAD_LOWERCASE_NAME}/translations")

set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM true)
