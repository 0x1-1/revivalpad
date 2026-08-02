## RevivalPad tests
##
## Enabled with -DWITH_TESTS=ON.

find_package(Qt${QT_VERSION_MAJOR} ${QT_REQUIRED_VERSION} COMPONENTS Test REQUIRED)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

###############################
# REBRAND / IDENTITY TESTS
###############################
# Self-contained: these exercise the product identity, the settings migration
# and the on-disk profile contract without needing the GUI.

add_executable(revivalpad_identity_tests
        testproductidentity.cpp
        )
target_link_libraries(revivalpad_identity_tests
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        ${SDL2_LIBRARIES})
target_include_directories(revivalpad_identity_tests PRIVATE ${SDL2_INCLUDE_DIRS})
add_test(NAME ProductIdentity COMMAND revivalpad_identity_tests)

add_executable(revivalpad_migration_tests
        testsettingsmigration.cpp
        ${PROJECT_SOURCE_DIR}/src/settingsmigration.cpp
        )
target_link_libraries(revivalpad_migration_tests
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        ${SDL2_LIBRARIES})
target_include_directories(revivalpad_migration_tests PRIVATE ${SDL2_INCLUDE_DIRS})
add_test(NAME SettingsMigration COMMAND revivalpad_migration_tests)

add_executable(revivalpad_profile_tests
        testprofilecompatibility.cpp
        )
target_compile_definitions(revivalpad_profile_tests PRIVATE
        REVIVALPAD_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
target_link_libraries(revivalpad_profile_tests
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        ${SDL2_LIBRARIES})
target_include_directories(revivalpad_profile_tests PRIVATE ${SDL2_INCLUDE_DIRS})
add_test(NAME ProfileCompatibility COMMAND revivalpad_profile_tests)

###############################
# LEGACY GUI TESTS
###############################
# Inherited from AntiMicroX. These have never been linkable: every source file
# declares its own QTEST_MAIN, and upstream left the target without any
# target_link_libraries() call. They are kept rather than deleted so the
# coverage they describe is not lost, but they are excluded from the default
# build and from ctest until the application is factored into a library they
# can link against. Enable with -DWITH_GUI_TESTS=ON to work on them.
option(WITH_GUI_TESTS "Build the inherited (currently non-linking) GUI tests" OFF)

set(GUIS_SRCS testaboutdialog.cpp
        testaddeditautoprofiledialog.cpp
        testadvancebuttondialog.cpp
        testcalibration.cpp
        testjoycontrolstickeditdialog.cpp
        testbuttoneditdialog.cpp
        testquicksetdialog.cpp
        testqkeydisplaydialog.cpp
        )

if(WITH_GUI_TESTS)
    add_executable(GuiTests ${GUIS_SRCS})
    target_link_libraries(GuiTests Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Widgets)
    ADD_TEST(NAME GuiTests COMMAND GuiTests)
endif(WITH_GUI_TESTS)
