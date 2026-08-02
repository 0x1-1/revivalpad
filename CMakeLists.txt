## antimicrox Gamepad to KB+M event mapper
## Copyright (C) 2015 Travis Nickles <nickles.travis@gmail.com>
## Copyright (C) 2020 Jagoda Górska <juliagoda.pl@protonmail.com>
## Copyright (C) 2020 Paweł Kotiuk <kotiuk@zohomail.eu>
## Copyright (C) 2026 0x1-1 (RevivalPad fork modifications)
##
## RevivalPad is a modified fork of AntiMicroX. See FORK_NOTICE.md.
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 3.12)

###############################
# REVIVALPAD PRODUCT IDENTITY
###############################
# Single source of truth for every product-facing identifier.
# These values are propagated into the C++ sources through src/config.h.in
# and into the Linux/Windows metadata through configure_file() below.

# The version number.
set(REVIVALPAD_MAJOR_VERSION 1)
set(REVIVALPAD_MINOR_VERSION 0)
set(REVIVALPAD_PATCH_VERSION 0)
set(REVIVALPAD_VERSION "${REVIVALPAD_MAJOR_VERSION}.${REVIVALPAD_MINOR_VERSION}.${REVIVALPAD_PATCH_VERSION}")

set(REVIVALPAD_DISPLAY_NAME "RevivalPad")
set(REVIVALPAD_LOWERCASE_NAME "revivalpad")
set(REVIVALPAD_EXECUTABLE_NAME "revivalpad")
set(REVIVALPAD_PRODUCT_FAMILY "Revival")
set(REVIVALPAD_PUBLISHER "0x1-1")
set(REVIVALPAD_DESCRIPTION "Map gamepad inputs to keyboard, mouse, macros and application actions.")
set(REVIVALPAD_SHORT_DESCRIPTION "Gamepad input mapper")

# ---------------------------------------------------------------------------
# FORK OWNER
# ---------------------------------------------------------------------------
# The GitHub account hosting this fork. Everything derived from it - repository
# id, homepage, issue tracker, update endpoint, Linux application id, D-Bus
# name, desktop/AppStream/MIME file names - follows automatically, so this is
# the only place the owner is written down. Override for a different fork with:
#
#     cmake -S . -B build -DREVIVALPAD_GITHUB_OWNER=<github-account>
# ---------------------------------------------------------------------------
set(REVIVALPAD_GITHUB_OWNER "0x1-1" CACHE STRING
    "GitHub account owning the RevivalPad fork")

set(REVIVALPAD_GITHUB_REPO "${REVIVALPAD_GITHUB_OWNER}/${REVIVALPAD_LOWERCASE_NAME}")

# The application id is reverse-DNS, so the owner has to be escaped before it
# can be used as a component. AppStream ids and - more strictly - D-Bus
# interface names only accept [A-Za-z_][A-Za-z0-9_]*: a leading digit and
# hyphens are both illegal. Apply the conventional Flathub escaping:
#   hyphens -> underscores, and a leading digit gets an underscore prefix.
# For owner "0x1-1" this yields the application id io.github._0x1_1.revivalpad.
string(TOLOWER "${REVIVALPAD_GITHUB_OWNER}" REVIVALPAD_APP_ID_OWNER)
string(REGEX REPLACE "[^a-z0-9_]" "_" REVIVALPAD_APP_ID_OWNER "${REVIVALPAD_APP_ID_OWNER}")
if(REVIVALPAD_APP_ID_OWNER MATCHES "^[0-9]")
    set(REVIVALPAD_APP_ID_OWNER "_${REVIVALPAD_APP_ID_OWNER}")
endif()
if(NOT REVIVALPAD_APP_ID_OWNER MATCHES "^[a-z_][a-z0-9_]*$")
    message(FATAL_ERROR
        "Could not derive a valid application id component from REVIVALPAD_GITHUB_OWNER "
        "('${REVIVALPAD_GITHUB_OWNER}').")
endif()

set(REVIVALPAD_APP_ID "io.github.${REVIVALPAD_APP_ID_OWNER}.${REVIVALPAD_LOWERCASE_NAME}")
set(REVIVALPAD_HOMEPAGE "https://github.com/${REVIVALPAD_GITHUB_REPO}")
set(REVIVALPAD_ISSUES_URL "${REVIVALPAD_HOMEPAGE}/issues")
set(REVIVALPAD_WIKI_URL "${REVIVALPAD_HOMEPAGE}/wiki")
set(REVIVALPAD_RAW_CONTENT_URL "https://raw.githubusercontent.com/${REVIVALPAD_GITHUB_REPO}/master")
set(REVIVALPAD_UPDATE_API_URL "https://api.github.com/repos/${REVIVALPAD_GITHUB_REPO}/releases/latest")
set(REVIVALPAD_LATEST_RELEASE_URL "${REVIVALPAD_HOMEPAGE}/releases/latest")

# Upstream attribution - see FORK_NOTICE.md. Never present these as RevivalPad's own.
set(REVIVALPAD_UPSTREAM_NAME "AntiMicroX")
set(REVIVALPAD_UPSTREAM_BASE_VERSION "3.6.1")
set(REVIVALPAD_UPSTREAM_REPO "AntiMicroX/antimicrox")
set(REVIVALPAD_UPSTREAM_URL "https://github.com/AntiMicroX/antimicrox")
set(REVIVALPAD_UPSTREAM_COMMIT "e0e11628aa6f20ddbce32caba496aa12ef8f84d3")

project(${REVIVALPAD_LOWERCASE_NAME}
        VERSION ${REVIVALPAD_VERSION}
        DESCRIPTION "${REVIVALPAD_DESCRIPTION}"
        HOMEPAGE_URL "${REVIVALPAD_HOMEPAGE}"
        LANGUAGES C CXX)

if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" "${CMAKE_ROOT}/Modules")

set(CMAKE_INCLUDE_CURRENT_DIR ON)


if(UNIX AND NOT APPLE AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "force cmake prefix to be set for /usr" FORCE)
endif(UNIX AND NOT APPLE AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND MSVC_TOOLSET_VERSION GREATER_EQUAL 141)
    set(COMPILER_IS_MSVC TRUE)
endif()

if(NOT COMPILER_IS_MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wcast-align -Wunreachable-code -g -O0 -fno-omit-frame-pointer")
    # ADDITIONAL DEBUG FLAGS
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused  -Wswitch-enum")
    if(UNIX AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -rdynamic")
    endif(UNIX AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /analyze- /D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /GS /Od /sdl /wd4996 /RTC1 /Gd /Oy-  /permissive-")
endif(NOT COMPILER_IS_MSVC)

if(UNIX)
    option(WITH_X11 "Compile with support for X11." ON)
    option(WITH_UINPUT "Compile with support for uinput. uinput will be usable to simulate events." ON)
    option(INSTALL_UINPUT_UDEV_RULES "Generate udev rules allowing users using uinput without root permissions." ON)
    option(WITH_XTEST "Compile with support for XTest.  XTest will be usable to simulate events." ON)
    option(APPDATA "Build project with AppData file support." ON)
endif(UNIX)

if(WIN32)
    option(PORTABLE_WIN "Compile app for portability" OFF)
endif(WIN32)

option(USE_QT6_BY_DEFAULT "Use Qt6 if available (if not available fallback to QT5)" ON)
option(UPDATE_TRANSLATIONS "Call lupdate to update translation files from source." OFF)
option(TRANS_REMOVE_OBSOLETE "Add -noobsolete option to lupdate command to get rid of old text entries" OFF)
option(ATTACH_FAKE_CLASSES "Fake classes can be used in application to tests functionalities" OFF)
option(REVIVALPAD_PKG_VERSION "Manually define version of package displayed in info tab" OFF)
option(CHECK_FOR_UPDATES "Enable checking for updates using GitHub REST API." OFF)
option(BUILD_DOCS "Build documentation" OFF)
option(WITH_TESTS "Allow tests for classes" OFF)

if(WITH_TESTS)
    message("Tests enabled")
endif(WITH_TESTS)

if(REVIVALPAD_PKG_VERSION)
    message("This build will be marked as: ${REVIVALPAD_PKG_VERSION}")
    add_compile_definitions(REVIVALPAD_PKG_VERSION="${REVIVALPAD_PKG_VERSION}")
endif(REVIVALPAD_PKG_VERSION)

if(CHECK_FOR_UPDATES)
    message("This build will check for updates using GitHub API.")
    add_compile_definitions(CHECK_FOR_UPDATES="${CHECK_FOR_UPDATES}")
endif(CHECK_FOR_UPDATES)

if(UNIX)
    if(WITH_XTEST AND NOT WITH_X11)
        set(WITH_XTEST OFF)
        message("Cannot use XTest without X11. Disabling XTest support.")
    endif(WITH_XTEST AND NOT WITH_X11)

    if(NOT WITH_X11 AND NOT WITH_UINPUT)
        message(FATAL_ERROR "No system is defined for simulating events. At least one of WITH_X11 or WITH_UINPUT must be enabled.")
    endif(NOT WITH_X11 AND NOT WITH_UINPUT)

    if(WITH_XTEST)
        message("XTest support allowed for simulating events.")
    endif(WITH_XTEST)

    if(WITH_UINPUT)
        message("uinput support allowed for simulating events.")
    else()
        set(INSTALL_UINPUT_UDEV_RULES OFF)
    endif(WITH_UINPUT)

    if(NOT WITH_XTEST AND NOT WITH_UINPUT)
        message(FATAL_ERROR "No system is defined for simulating events.")
    endif(NOT WITH_XTEST AND NOT WITH_UINPUT)
endif(UNIX)

###############################
# PLATFORM-INDEPENDENT FILES
###############################

set(revivalpad_MAIN src/main.cpp)

set(revivalpad_SOURCES
        src/revivalpadsettings.cpp
        src/antkeymapper.cpp
        src/applaunchhelper.cpp
        src/autoprofileinfo.cpp
        src/gui/axisvaluebox.cpp
        src/commandlineutility.cpp
        src/common.cpp
        src/dpadcontextmenu.cpp
        src/dpadpushbutton.cpp
        src/dpadpushbuttongroup.cpp
        src/event.cpp
        src/eventhandlerfactory.cpp
        src/eventhandlers/baseeventhandler.cpp
        src/gamecontroller/gamecontroller.cpp
        src/gamecontroller/gamecontrollerdpad.cpp
        src/gamecontroller/gamecontrollerset.cpp
        src/gamecontroller/gamecontrollertrigger.cpp
        src/gamecontroller/gamecontrollertriggerbutton.cpp
        src/gamecontrollerexample.cpp
        src/globalvariables.cpp
        src/gui/aboutdialog.cpp
        src/gui/addeditautoprofiledialog.cpp
        src/gui/advancebuttondialog.cpp
        src/gui/advancestickassignmentdialog.cpp
        src/gui/axiseditdialog.cpp
        src/gui/buttoneditdialog.cpp
        src/gui/calibration.cpp
        src/gui/dpadeditdialog.cpp
        src/gui/editalldefaultautoprofiledialog.cpp
        src/gui/extraprofilesettingsdialog.cpp
        src/gui/flashbuttonwidget.cpp
        src/gui/gamecontrollermappingdialog.cpp
        src/gui/joyaxiswidget.cpp
        src/gui/joybuttonwidget.cpp
        src/gui/joycontrolstickeditdialog.cpp
        src/gui/joydpadbuttonwidget.cpp
        src/gui/joysensoreditdialog.cpp
        src/gui/joystickstatuswindow.cpp
        src/gui/joytabwidget.cpp
        src/gui/joytabwidgetcontainer.cpp
        src/gui/mainsettingsdialog.cpp
        src/gui/mainwindow.cpp
        src/gui/mousesettingsdialog.cpp
        src/gui/qkeydisplaydialog.cpp
        src/gui/quicksetdialog.cpp
        src/gui/setaxisthrottledialog.cpp
        src/gui/setnamesdialog.cpp
        src/gui/slotitemlistwidget.cpp
        src/haptictriggerps5.cpp
        src/inputdaemon.cpp
        src/inputdevice.cpp
        src/inputdevicebitarraystatus.cpp
        src/inputdevicecalibration.cpp
        src/sensors/joyaccelerometersensor.cpp
        src/joyaxis.cpp
        src/joyaxiscontextmenu.cpp
        src/gui/joybuttoncontextmenu.cpp
        src/gui/joybuttonmousehelper.cpp
        src/gui/joybuttonslot.cpp
        src/gui/joybuttonstatusbox.cpp
        src/joybuttontypes/joybutton.cpp
        src/joybuttontypes/joyaccelerometerbutton.cpp
        src/joybuttontypes/joyaxisbutton.cpp
        src/joybuttontypes/joycontrolstickbutton.cpp
        src/joybuttontypes/joycontrolstickmodifierbutton.cpp
        src/joybuttontypes/joydpadbutton.cpp
        src/joybuttontypes/joygradientbutton.cpp
        src/joybuttontypes/joygyroscopebutton.cpp
        src/joybuttontypes/joysensorbutton.cpp
        src/joycontrolstick.cpp
        src/joycontrolstickbuttonpushbutton.cpp
        src/joycontrolstickcontextmenu.cpp
        src/joycontrolstickpushbutton.cpp
        src/joycontrolstickstatusbox.cpp
        src/joydpad.cpp
        src/sensors/joygyroscopesensor.cpp
        src/sensors/joysensor.cpp
        src/sensors/joysensorbuttonpushbutton.cpp
        src/sensors/joysensorcontextmenu.cpp
        src/sensors/joysensorfactory.cpp
        src/sensors/joysensorpreset.cpp
        src/sensors/joysensorpushbutton.cpp
        src/sensors/joysensorstatusbox.cpp
        src/joystick.cpp
        src/keyboard/virtualkeyboardmousewidget.cpp
        src/keyboard/virtualkeypushbutton.cpp
        src/keyboard/virtualmousepushbutton.cpp
        src/localrevivalpadserver.cpp
        src/logger.cpp
        src/mousedialog/mouseaxissettingsdialog.cpp
        src/mousedialog/mousebuttonsettingsdialog.cpp
        src/mousedialog/mousecontrolsticksettingsdialog.cpp
        src/mousedialog/mousedpadsettingsdialog.cpp
        src/mousedialog/mousesensorsettingsdialog.cpp
        src/mousedialog/springmoderegionpreview.cpp
        src/mousedialog/uihelpers/mouseaxissettingsdialoghelper.cpp
        src/mousedialog/uihelpers/mousebuttonsettingsdialoghelper.cpp
        src/mousedialog/uihelpers/mousecontrolsticksettingsdialoghelper.cpp
        src/mousedialog/uihelpers/mousedpadsettingsdialoghelper.cpp
        src/mousehelper.cpp
        src/pt1filter.cpp
        src/qtkeymapperbase.cpp
        src/sdleventreader.cpp
        src/settingsmigration.cpp
        src/gui/sensorpushbuttongroup.cpp
        src/setjoystick.cpp
        src/gui/simplekeygrabberbutton.cpp
        src/statisticsestimator.cpp
        src/gui/stickpushbuttongroup.cpp
        src/uihelpers/advancebuttondialoghelper.cpp
        src/uihelpers/buttoneditdialoghelper.cpp
        src/uihelpers/dpadcontextmenuhelper.cpp
        src/uihelpers/dpadeditdialoghelper.cpp
        src/uihelpers/gamecontrollermappingdialoghelper.cpp
        src/uihelpers/joyaxiscontextmenuhelper.cpp
        src/uihelpers/joycontrolstickcontextmenuhelper.cpp
        src/uihelpers/joycontrolstickeditdialoghelper.cpp
        src/uihelpers/joysensoriothreadhelper.cpp
        src/uihelpers/joytabwidgethelper.cpp
        src/vdpad.cpp
        src/xml/inputdevicexml.cpp
        src/xml/joyaxisxml.cpp
        src/xml/joybuttonslotxml.cpp
        src/xml/joybuttonxml.cpp
        src/xml/joydpadxml.cpp
        src/xml/setjoystickxml.cpp
        src/xmlconfigmigration.cpp
        src/xmlconfigreader.cpp
        src/xmlconfigwriter.cpp
        )

set(revivalpad_HEADERS
        src/revivalpadsettings.h
        src/antkeymapper.h
        src/applaunchhelper.h
        src/autoprofileinfo.h
        src/gui/axisvaluebox.h
        src/commandlineutility.h
        src/dpadcontextmenu.h
        src/dpadpushbutton.h
        src/dpadpushbuttongroup.h
        src/eventhandlerfactory.h
        src/eventhandlers/baseeventhandler.h
        src/gamecontroller/gamecontroller.h
        src/gamecontroller/gamecontrollerdpad.h
        src/gamecontroller/gamecontrollerset.h
        src/gamecontroller/gamecontrollertrigger.h
        src/gamecontroller/gamecontrollertriggerbutton.h
        src/gamecontrollerexample.h
        src/globalvariables.h
        src/gui/aboutdialog.h
        src/gui/addeditautoprofiledialog.h
        src/gui/advancebuttondialog.h
        src/gui/advancestickassignmentdialog.h
        src/gui/axiseditdialog.h
        src/gui/buttoneditdialog.h
        src/gui/calibration.h
        src/gui/dpadeditdialog.h
        src/gui/editalldefaultautoprofiledialog.h
        src/gui/extraprofilesettingsdialog.h
        src/gui/flashbuttonwidget.h
        src/gui/gamecontrollermappingdialog.h
        src/gui/joyaxiswidget.h
        src/gui/joybuttonwidget.h
        src/gui/joycontrolstickeditdialog.h
        src/gui/joydpadbuttonwidget.h
        src/gui/joysensoreditdialog.h
        src/gui/joystickstatuswindow.h
        src/gui/joytabwidget.h
        src/gui/joytabwidgetcontainer.h
        src/gui/mainsettingsdialog.h
        src/gui/mainwindow.h
        src/gui/mousesettingsdialog.h
        src/gui/qkeydisplaydialog.h
        src/gui/quicksetdialog.h
        src/gui/setaxisthrottledialog.h
        src/gui/setnamesdialog.h
        src/gui/slotitemlistwidget.h
        src/haptictriggerps5.h
        src/haptictriggermodeps5.h
        src/inputdaemon.h
        src/inputdevice.h
        src/inputdevicebitarraystatus.h
        src/inputdevicecalibration.h
        src/sensors/joyaccelerometersensor.h
        src/joyaxis.h
        src/joyaxiscontextmenu.h
        src/gui/joybuttoncontextmenu.h
        src/gui/joybuttonmousehelper.h
        src/gui/joybuttonslot.h
        src/gui/joybuttonstatusbox.h
        src/joybuttontypes/joybutton.h
        src/joybuttontypes/joyaccelerometerbutton.h
        src/joybuttontypes/joyaxisbutton.h
        src/joybuttontypes/joycontrolstickbutton.h
        src/joybuttontypes/joycontrolstickmodifierbutton.h
        src/joybuttontypes/joydpadbutton.h
        src/joybuttontypes/joygradientbutton.h
        src/joybuttontypes/joygyroscopebutton.h
        src/joybuttontypes/joysensorbutton.h
        src/joycontrolstick.h
        src/joycontrolstickbuttonpushbutton.h
        src/joycontrolstickcontextmenu.h
        src/joycontrolstickpushbutton.h
        src/joycontrolstickstatusbox.h
        src/joydpad.h
        src/sensors/joygyroscopesensor.h
        src/sensors/joysensor.h
        src/sensors/joysensorbuttonpushbutton.h
        src/sensors/joysensorcontextmenu.h
        src/sensors/joysensordirection.h
        src/sensors/joysensorfactory.h
        src/sensors/joysensorpreset.h
        src/sensors/joysensorpushbutton.h
        src/sensors/joysensorstatusbox.h
        src/sensors/joysensortype.h
        src/joystick.h
        src/keyboard/virtualkeyboardmousewidget.h
        src/keyboard/virtualkeypushbutton.h
        src/keyboard/virtualmousepushbutton.h
        src/localrevivalpadserver.h
        src/logger.h
        src/mousedialog/mouseaxissettingsdialog.h
        src/mousedialog/mousebuttonsettingsdialog.h
        src/mousedialog/mousecontrolsticksettingsdialog.h
        src/mousedialog/mousedpadsettingsdialog.h
        src/mousedialog/springmoderegionpreview.h
        src/mousedialog/mousesensorsettingsdialog.h
        src/mousedialog/uihelpers/mouseaxissettingsdialoghelper.h
        src/mousedialog/uihelpers/mousebuttonsettingsdialoghelper.h
        src/mousedialog/uihelpers/mousecontrolsticksettingsdialoghelper.h
        src/mousedialog/uihelpers/mousedpadsettingsdialoghelper.h
        src/mousehelper.h
        src/pt1filter.h
        src/qtkeymapperbase.h
        src/sdleventreader.h
        src/settingsmigration.h
        src/gui/sensorpushbuttongroup.h
        src/setjoystick.h
        src/gui/simplekeygrabberbutton.h
        src/statisticsestimator.h
        src/gui/stickpushbuttongroup.h
        src/uihelpers/advancebuttondialoghelper.h
        src/uihelpers/buttoneditdialoghelper.h
        src/uihelpers/dpadcontextmenuhelper.h
        src/uihelpers/dpadeditdialoghelper.h
        src/uihelpers/gamecontrollermappingdialoghelper.h
        src/uihelpers/joyaxiscontextmenuhelper.h
        src/uihelpers/joycontrolstickcontextmenuhelper.h
        src/uihelpers/joycontrolstickeditdialoghelper.h
        src/uihelpers/joysensoriothreadhelper.h
        src/uihelpers/joytabwidgethelper.h
        src/vdpad.h
        src/xml/inputdevicexml.h
        src/xml/joyaxisxml.h
        src/xml/joybuttonslotxml.h
        src/xml/joybuttonxml.h
        src/xml/joydpadxml.h
        src/xml/setjoystickxml.h
        src/xmlconfigmigration.h
        src/xmlconfigreader.h
        src/xmlconfigwriter.h
        )

if(ATTACH_FAKE_CLASSES)
    LIST(APPEND revivalpad_SOURCES
            src/fakeclasses/xbox360wireless.cpp
            )

    LIST(APPEND revivalpad_HEADERS
            src/fakeclasses/xbox360wireless.h
            )
endif(ATTACH_FAKE_CLASSES)

set(revivalpad_FORMS
        src/gui/aboutdialog.ui
        src/gui/addeditautoprofiledialog.ui
        src/gui/advancebuttondialog.ui
        src/gui/advancestickassignmentdialog.ui
        src/gui/axiseditdialog.ui
        src/gui/buttoneditdialog.ui
        src/gui/calibration.ui
        src/gui/capturedwindowinfodialog.ui
        src/gui/dpadeditdialog.ui
        src/gui/editalldefaultautoprofiledialog.ui
        src/gui/extraprofilesettingsdialog.ui
        src/gui/gamecontrollermappingdialog.ui
        src/gui/joycontrolstickeditdialog.ui
        src/gui/joysensoreditdialog.ui
        src/gui/joystickstatuswindow.ui
        src/gui/mainsettingsdialog.ui
        src/gui/mainwindow.ui
        src/gui/mousesettingsdialog.ui
        src/gui/qkeydisplaydialog.ui
        src/gui/quicksetdialog.ui
        src/gui/setaxisthrottledialog.ui
        src/gui/setnamesdialog.ui
        )

set(revivalpad_RESOURCES src/resources.qrc)

###############################
# PLATFORM-DEPENDENT FILES
###############################

if(UNIX)
    if(WITH_X11)
        LIST(APPEND revivalpad_SOURCES src/x11extras.cpp
                src/qtx11keymapper.cpp
                src/unixcapturewindowutility.cpp
                src/autoprofilewatcher.cpp
                src/gui/capturedwindowinfodialog.cpp
                )
        LIST(APPEND revivalpad_HEADERS src/x11extras.h
                src/qtx11keymapper.h
                src/unixcapturewindowutility.h
                src/autoprofilewatcher.h
                src/gui/capturedwindowinfodialog.h
                )

        if(WITH_XTEST)
            LIST(APPEND revivalpad_SOURCES src/eventhandlers/xtesteventhandler.cpp)
            LIST(APPEND revivalpad_HEADERS src/eventhandlers/xtesteventhandler.h)
        endif(WITH_XTEST)
    endif(WITH_X11)

    if(WITH_UINPUT)
        LIST(APPEND revivalpad_SOURCES src/qtuinputkeymapper.cpp
                src/uinputhelper.cpp
                src/eventhandlers/uinputeventhandler.cpp
                )
        LIST(APPEND revivalpad_HEADERS src/qtuinputkeymapper.h
                src/uinputhelper.h
                src/eventhandlers/uinputeventhandler.h
                )
    endif(WITH_UINPUT)

elseif(WIN32)
    LIST(APPEND revivalpad_SOURCES
        src/autoprofilewatcher.cpp
        src/winextras.cpp
         src/qtwinkeymapper.cpp
         src/gui/winappprofiletimerdialog.cpp
         src/gui/capturedwindowinfodialog.cpp
         src/eventhandlers/winsendinputeventhandler.cpp
         src/joykeyrepeathelper.cpp
    )
    LIST(APPEND revivalpad_HEADERS
        src/autoprofilewatcher.h
        src/winextras.h
        src/qtwinkeymapper.h
        src/gui/winappprofiletimerdialog.h
        src/gui/capturedwindowinfodialog.h
        src/eventhandlers/winsendinputeventhandler.h
        src/joykeyrepeathelper.h
    )
    LIST(APPEND revivalpad_FORMS
        src/gui/winappprofiletimerdialog.ui    
    )
endif(UNIX)

###############################
# DEFINITIONS
###############################

if(UNIX)
    if(WITH_X11)
        add_definitions(-DWITH_X11)
    endif(WITH_X11)

    if(WITH_XTEST)
        add_definitions(-DWITH_XTEST)
    endif(WITH_XTEST)

    if(WITH_UINPUT)
        add_definitions(-DWITH_UINPUT)
    endif(WITH_UINPUT)
endif(UNIX)

###############################
# REQUIRED PACKAGES
###############################

#QT
set(QT_REQUIRED_VERSION 5.10)
set(CMAKE_AUTOMOC TRUE) # required by moc preprocessor used in QT

if(USE_QT6_BY_DEFAULT)
    find_package(Qt6 COMPONENTS Core)
    set(Use_QT6 TRUE)
    if(NOT Qt6_FOUND)
        message("QT6 not found. Falling back to QT5.")
        set(Use_QT6 FALSE)
    endif()
else()
    find_package(Qt5 COMPONENTS Core)
    set(Use_QT6 FALSE)
    if(NOT Qt5_FOUND)
        message("QT5 not found. Falling back to QT6.")
        set(Use_QT6 TRUE)
    endif()
endif()

if (Use_QT6)
    find_package(Qt6 ${QT_REQUIRED_VERSION} 
        COMPONENTS Core Widgets Gui Network LinguistTools Concurrent
        REQUIRED)
    find_package(QT NAMES Qt6 REQUIRED) # used to get version number in QT_VERSION_MAJOR and QT_VERSION

    QT_WRAP_UI(revivalpad_FORMS_HEADERS ${revivalpad_FORMS})
    QT_ADD_RESOURCES(revivalpad_RESOURCES_RCC ${revivalpad_RESOURCES})

    if(UNIX)
        find_package(Qt6 ${QT_REQUIRED_VERSION} COMPONENTS DBus REQUIRED)
        configure_file(
            "${PROJECT_SOURCE_DIR}/other/io.github._0x1_1.revivalpad.inputdevice.xml.in"
            "${PROJECT_BINARY_DIR}/dbus/${REVIVALPAD_APP_ID}.inputdevice.xml"
            @ONLY)
        QT_ADD_DBUS_ADAPTOR(revivalpad_FORMS_HEADERS
            "${PROJECT_BINARY_DIR}/dbus/${REVIVALPAD_APP_ID}.inputdevice.xml" inputdevice.h InputDevice)
        LIST(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::DBus)
    endif()
else()
    find_package(Qt5 ${QT_REQUIRED_VERSION} 
        COMPONENTS Core Widgets Gui Network LinguistTools Concurrent
        REQUIRED)
    find_package(QT NAMES Qt5 REQUIRED) # used to get version number in QT_VERSION_MAJOR and QT_VERSION

    QT5_WRAP_UI(revivalpad_FORMS_HEADERS ${revivalpad_FORMS})
    QT5_ADD_RESOURCES(revivalpad_RESOURCES_RCC ${revivalpad_RESOURCES})
endif()
message("Found QT: ${QT_VERSION}")

add_subdirectory("share/${REVIVALPAD_LOWERCASE_NAME}/translations")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# SDL
set(SDL_REQUIRED_VERSION 2.0.6)
find_package(SDL2 REQUIRED)

# Others
if(WITH_X11)
    find_package(X11 REQUIRED)
endif(WITH_X11)

###############################
# LINKER LIBRARIES
###############################

LIST(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::Core)
LIST(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::Gui)
LIST(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::Widgets)
LIST(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::Concurrent)
LIST(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::Network)

if(UNIX)
    if(WITH_X11)
        LIST(APPEND X11_LIBS ${X11_X11_LIB})
        LIST(APPEND X11_LIBS ${X11_Xi_LIB})
    endif(WITH_X11)

    if(WITH_XTEST)
        LIST(APPEND EXTRA_LIBS ${X11_XTest_LIB})
    endif(WITH_XTEST)

     # necessary ifwe use find_package for SDL2
     #    if(NOT DEFINED SDL2_LIBRARIES)
#        set(SDL2_LIBRARIES SDL2::SDL2)
#    endif()
elseif (WIN32)
    list(APPEND WIN_LIBS "psapi")
endif(UNIX)


include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        "${CMAKE_CURRENT_BINARY_DIR}/src"
        ${PROJECT_SOURCE_DIR}
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/src/sensors"
        "${PROJECT_SOURCE_DIR}/src/gui"
        ${SDL2_INCLUDE_DIRS}
)


# Store executable in a bin subdir. Needed here so translations can be loaded.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

###############################
# COMPILE & LINK
###############################
if(UNIX)
    add_executable(${REVIVALPAD_EXECUTABLE_NAME}
        ${revivalpad_MAIN}
        ${revivalpad_HEADERS_MOC}
        ${revivalpad_SOURCES}
        ${revivalpad_FORMS_HEADERS}
        ${revivalpad_RESOURCES_RCC}
        )
elseif(WIN32)
    # Embedded icon and VERSIONINFO (ProductName, FileDescription, InternalName,
    # OriginalFilename, ...) are generated so they always follow the identity block.
    configure_file(
        "${PROJECT_SOURCE_DIR}/src/revivalpad_windows_resource.rc.in"
        "${PROJECT_BINARY_DIR}/revivalpad_windows_resource.rc"
        @ONLY)
    # The .rc references images/revivalpad.ico relative to src/.
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -I\"${PROJECT_SOURCE_DIR}/src\"")

    # The WIN32 is required to specify a GUI application.
    add_executable(${REVIVALPAD_EXECUTABLE_NAME} WIN32
        ${revivalpad_MAIN}
        ${revivalpad_HEADERS_MOC}
        ${revivalpad_SOURCES}
        ${revivalpad_FORMS_HEADERS}
        ${revivalpad_RESOURCES_RCC}
        "${PROJECT_BINARY_DIR}/revivalpad_windows_resource.rc" # icon + VERSIONINFO metadata
    )
    target_link_libraries(${REVIVALPAD_EXECUTABLE_NAME}
        ${WIN_LIBS}
    )

    if(PORTABLE_WIN)
        add_definitions(-DWIN_PORTABLE_PACKAGE)
    endif()
endif(UNIX)

target_link_libraries(${REVIVALPAD_EXECUTABLE_NAME}
        ${QT_LIBS}
        ${X11_LIBS}
        ${SDL2_LIBRARIES}
        ${EXTRA_LIBS}
        )

target_include_directories(${REVIVALPAD_EXECUTABLE_NAME} PUBLIC
        ${SDL2_INCLUDE_DIRS}/SDL2
        )

###############################
# INSTALL
###############################

# Specify out directory for final executable.
install(TARGETS ${REVIVALPAD_EXECUTABLE_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

if(UNIX)
    find_package(ECM REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_DIR})
    include(ECMInstallIcons)

    ecm_install_icons(ICONS share/icons/application/16x16/apps/16-apps-io.github._0x1_1.revivalpad.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    ecm_install_icons(ICONS share/icons/application/24x24/apps/24-apps-io.github._0x1_1.revivalpad.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    ecm_install_icons(ICONS share/icons/application/32x32/apps/32-apps-io.github._0x1_1.revivalpad.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    ecm_install_icons(ICONS share/icons/application/48x48/apps/48-apps-io.github._0x1_1.revivalpad.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    ecm_install_icons(ICONS share/icons/application/64x64/apps/64-apps-io.github._0x1_1.revivalpad.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    ecm_install_icons(ICONS share/icons/application/128x128/apps/128-apps-io.github._0x1_1.revivalpad.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    ecm_install_icons(ICONS share/icons/application/256x256/apps/256-apps-io.github._0x1_1.revivalpad.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    ecm_install_icons(ICONS share/icons/application/scalable/apps/sc-apps-io.github._0x1_1.revivalpad.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)

    ecm_install_icons(ICONS share/icons/breeze_themed/48-apps-io.github._0x1_1.revivalpad.trayicon.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons THEME breeze)
    ecm_install_icons(ICONS src/images/48-apps-io.github._0x1_1.revivalpad.trayicon.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)

    install(FILES CHANGELOG.md FORK_NOTICE.md DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/doc/${REVIVALPAD_LOWERCASE_NAME}")

    # Desktop entry and MIME package are generated so that their file names and
    # contents always follow REVIVALPAD_APP_ID.
    configure_file(
        "${PROJECT_SOURCE_DIR}/other/io.github._0x1_1.revivalpad.desktop.in"
        "${PROJECT_BINARY_DIR}/linux/${REVIVALPAD_APP_ID}.desktop"
        @ONLY)
    configure_file(
        "${PROJECT_SOURCE_DIR}/other/io.github._0x1_1.revivalpad.xml.in"
        "${PROJECT_BINARY_DIR}/linux/${REVIVALPAD_APP_ID}.xml"
        @ONLY)
    install(FILES "${PROJECT_BINARY_DIR}/linux/${REVIVALPAD_APP_ID}.desktop"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
    install(FILES "${PROJECT_BINARY_DIR}/linux/${REVIVALPAD_APP_ID}.xml"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/mime/packages")

    # Copy current Changelog file to location that the resource file expects.
    install(FILES CHANGELOG.md DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${REVIVALPAD_LOWERCASE_NAME}")
endif(UNIX)

# Add man page for *nix platforms.
if(UNIX)
    add_subdirectory(other)

    # Only way to force install target to be dependent on manpage.
    install(CODE "execute_process(COMMAND ${CMAKE_BUILD_TOOL} manpage WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")

    if(APPDATA)
        # Only way to force install target to be dependent on appdata.
        install(CODE "execute_process(COMMAND ${CMAKE_BUILD_TOOL} appdata WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")
    endif(APPDATA)
endif(UNIX)

if(UNIX)
    if(INSTALL_UINPUT_UDEV_RULES)
        message("Udev rules installation enabled.")
        install(FILES other/60-revivalpad-uinput.rules DESTINATION "/usr/lib/udev/rules.d/")
    endif(INSTALL_UINPUT_UDEV_RULES)
endif(UNIX)

# Copy libraries needed by Windows
if(WIN32)
    add_custom_command(TARGET ${REVIVALPAD_EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "dll files linked with ${REVIVALPAD_EXECUTABLE_NAME} executable: $<TARGET_RUNTIME_DLLS:${REVIVALPAD_EXECUTABLE_NAME}> "
        COMMAND_EXPAND_LISTS
        )
    add_custom_command(TARGET ${REVIVALPAD_EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${REVIVALPAD_EXECUTABLE_NAME}> $<TARGET_FILE_DIR:${REVIVALPAD_EXECUTABLE_NAME}>
        COMMAND_EXPAND_LISTS
        )

    find_path(SDL2_DLL_LOCATION_DIR "SDL2.dll" HINTS "${SDL2_LIBRARY_DIRS}/../bin")

    #these three lines assume SDL2 in standard location TODO do it better
    find_path(GCC_DLL "libgcc_s_seh-1.dll" HINTS "${SDL2_LIBRARY_DIRS}/../bin")
    find_path(PTHREAD_DLL_DIR "libwinpthread-1.dll" HINTS "${SDL2_LIBRARY_DIRS}/../bin")
    find_path(LIBCPP_DLL_DIR "libstdc++-6.dll" HINTS "${SDL2_LIBRARY_DIRS}/../bin")
    list(APPEND DLL_FILES
        "${CMAKE_CURRENT_BINARY_DIR}/bin/Qt${QT_VERSION_MAJOR}Core.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/Qt${QT_VERSION_MAJOR}Gui.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/Qt${QT_VERSION_MAJOR}Concurrent.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/Qt${QT_VERSION_MAJOR}Network.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/Qt${QT_VERSION_MAJOR}Widgets.dll"
        "${SDL2_DLL_LOCATION_DIR}/SDL2.dll"
        "${GCC_DLL}/libgcc_s_seh-1.dll"
        "${PTHREAD_DLL_DIR}/libwinpthread-1.dll"
        "${LIBCPP_DLL_DIR}/libstdc++-6.dll"
        )

    if(QT_VERSION_MAJOR EQUAL 5)
        list(APPEND DLL_FILES
            "${CMAKE_CURRENT_BINARY_DIR}/bin/libEGL.dll"
            )
    endif(QT_VERSION_MAJOR EQUAL 5)
    

    if(CHECK_FOR_UPDATES)
        message("Searching for SSL DLLs.")

        find_file(LIBCRYPTO_DLL_FILE
            NAMES libcrypto-1_1-x64.dll libcrypto-3-x64.dll libcrypto-1_1.dll libcrypto-3.dll
            HINTS "${SDL2_LIBRARY_DIRS}/../bin" "${CMAKE_INSTALL_PREFIX}/bin" "$ENV{PATH}"
        )

        find_file(LIBSSL_DLL_FILE
            NAMES libssl-1_1-x64.dll libssl-3-x64.dll libssl-1_1.dll libssl-3.dll
            HINTS "${SDL2_LIBRARY_DIRS}/../bin" "${CMAKE_INSTALL_PREFIX}/bin" "$ENV{PATH}"
        )

        if(LIBCRYPTO_DLL_FILE AND LIBSSL_DLL_FILE)
            list(APPEND DLL_FILES
                "${LIBCRYPTO_DLL_FILE}"
                "${LIBSSL_DLL_FILE}"
            )
        else()
            message(WARNING "CHECK_FOR_UPDATES is enabled, but required OpenSSL DLL(s) were not found. "
                            "Package will be built without libcrypto/libssl files.")
        endif()
    endif(CHECK_FOR_UPDATES)

    install(FILES ${DLL_FILES} TYPE BIN )

    # Required platform plugin
    get_target_property(QtCore_LOCATION Qt${QT_VERSION_MAJOR}::Core LOCATION)
    get_filename_component(QT_DLL_DIR ${QtCore_LOCATION} PATH)
    install(FILES
        ${QT_DLL_DIR}/../plugins/platforms/qwindows.dll
        DESTINATION ./bin/platforms/)
endif(WIN32)

# uninstall target
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


# Only way to force install target to be dependent on updateqm.
install(CODE "execute_process(COMMAND ${CMAKE_BUILD_TOOL} updateqm WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")

# Use this to use some variables created here in the actual project.
# Modify the config.h.in file using the appropriate variables.
configure_file(
        "${PROJECT_SOURCE_DIR}/src/config.h.in"
        "${PROJECT_BINARY_DIR}/config.h"
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/)

if(WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(WITH_TESTS)

# Install SDL database with linked License file
if(UNIX)
    install(FILES share/gamecontrollerdb_linux.txt DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${REVIVALPAD_LOWERCASE_NAME}/" RENAME gamecontrollerdb.txt)
elseif(WIN32)
    install(FILES share/gamecontrollerdb_windows.txt DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${REVIVALPAD_LOWERCASE_NAME}/" RENAME gamecontrollerdb.txt)
endif(UNIX)
install(FILES share/LICENSE_SDL_GameControllerDB DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${REVIVALPAD_LOWERCASE_NAME}/")

###############################
# PACKAGES BUILDING WITH CPACK
###############################

include(InstallRequiredSystemLibraries)


set(CPACK_PACKAGE_NAME "${REVIVALPAD_LOWERCASE_NAME}")
set(CPACK_PACKAGE_VENDOR "${REVIVALPAD_PUBLISHER}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${REVIVALPAD_DESCRIPTION}")
set(CPACK_PACKAGE_HOMEPAGE_URL "${REVIVALPAD_HOMEPAGE}")


set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_VERSION_MAJOR "${REVIVALPAD_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${REVIVALPAD_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${REVIVALPAD_PATCH_VERSION}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CPACK_PACKAGE_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}-dbg")
endif()

# Artifact naming, e.g. revivalpad-1.0.0-windows-x86_64.exe
set(CPACK_PACKAGE_FILE_NAME "${REVIVALPAD_LOWERCASE_NAME}-${REVIVALPAD_VERSION}")
if(WIN32)
    if(PORTABLE_WIN)
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-windows-portable")
    else(PORTABLE_WIN)
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-windows")
    endif(PORTABLE_WIN)
endif(WIN32)
# Normalise the architecture token so artifact names are predictable across
# hosts: Windows reports AMD64/ARM64 where Linux reports x86_64/aarch64.
set(REVIVALPAD_ARCH "${CMAKE_HOST_SYSTEM_PROCESSOR}")
if(REVIVALPAD_ARCH MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    set(REVIVALPAD_ARCH "x86_64")
elseif(REVIVALPAD_ARCH MATCHES "^(ARM64|arm64|aarch64)$")
    set(REVIVALPAD_ARCH "aarch64")
endif()
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-${REVIVALPAD_ARCH}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${REVIVALPAD_DISPLAY_NAME}")

if(UNIX)
    set(CPACK_STRIP_FILES "")
    set(CPACK_SOURCE_STRIP_FILES "")
endif()

# Building .deb package
if(CPACK_GENERATOR STREQUAL "DEB")
    if(QT_VERSION_MAJOR EQUAL 5)
        # TODO use regular, not dev packages
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= ${QT_REQUIRED_VERSION}), libsdl2-2.0-0 (>= ${SDL_REQUIRED_VERSION}), libc6")
    else()
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6 (>= ${QT_REQUIRED_VERSION}), libsdl2-2.0-0 (>= ${SDL_REQUIRED_VERSION}), libc6")
    endif(QT_VERSION_MAJOR EQUAL 5)
    # Debian artifact naming, e.g. revivalpad_1.0.0_amd64.deb
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_PACKAGE_NAME "${REVIVALPAD_LOWERCASE_NAME}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${REVIVALPAD_PUBLISHER}")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${REVIVALPAD_HOMEPAGE}")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${REVIVALPAD_DISPLAY_NAME} is a graphical program used to map gamepad buttons to keyboard, mouse, scripts and macros.

It is a modified fork of AntiMicroX ${REVIVALPAD_UPSTREAM_BASE_VERSION}. See FORK_NOTICE.md.")

    message("Preparing documentation for DEB package")
    add_custom_target(package_docummentation ALL)

    #Compress changelog and save it as share/doc/revivalpad/changelog.gz
    add_custom_command(TARGET package_docummentation PRE_BUILD
            COMMAND gzip -c -9 -n "${PROJECT_SOURCE_DIR}/CHANGELOG.md" > "changelog.gz" VERBATIM)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz" DESTINATION "share/doc/${REVIVALPAD_LOWERCASE_NAME}/")

    #Strip binaries from unnecessary notes, comments, etc
    add_custom_command(TARGET ${REVIVALPAD_EXECUTABLE_NAME} POST_BUILD
            COMMAND strip --strip-unneeded --remove-section=.comment --remove-section=.note "${CMAKE_CURRENT_BINARY_DIR}/bin/${REVIVALPAD_EXECUTABLE_NAME}" VERBATIM)
endif()

if(WIN32 AND NOT UNIX)
    if(NOT PORTABLE_WIN)
        # We set NSIS as generator for Windows
        set(CPACK_GENERATOR NSIS)

        # There is a bug in NSI that does not handle full UNIX paths properly.
        # Make sure there is at least one set of four backlashes.
        set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/src/images\\\\revivalpad.ico")
        set(CPACK_NSIS_MUI_ICON "${CPACK_PACKAGE_ICON}")
        set(CPACK_NSIS_MUI_UNIICON "${CPACK_PACKAGE_ICON}")
        set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/${REVIVALPAD_EXECUTABLE_NAME}.exe")

        # Start Menu / Add-Remove Programs identity
        set(CPACK_NSIS_DISPLAY_NAME "${REVIVALPAD_DISPLAY_NAME}")
        set(CPACK_NSIS_PACKAGE_NAME "${REVIVALPAD_DISPLAY_NAME}")
        set(CPACK_NSIS_HELP_LINK "${REVIVALPAD_ISSUES_URL}")
        set(CPACK_NSIS_URL_INFO_ABOUT "${REVIVALPAD_HOMEPAGE}")
        set(CPACK_NSIS_CONTACT "${REVIVALPAD_ISSUES_URL}")
        set(CPACK_NSIS_MODIFY_PATH ON)
    else()
        set(CPACK_GENERATOR ZIP)
    endif()
endif()

set(CPACK_PACKAGE_EXECUTABLES "${REVIVALPAD_EXECUTABLE_NAME};${REVIVALPAD_DISPLAY_NAME}")
set(CPACK_CREATE_DESKTOP_LINKS "${REVIVALPAD_EXECUTABLE_NAME}")
include(CPack)


###############################
# DOCS
###############################

if(BUILD_DOCS)
    message("Doxygen build enabled")
    find_package(Doxygen)

    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/other/Doxyfile_config.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( doxygen_docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )

    else (DOXYGEN_FOUND)
      message(WARNING "Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif(BUILD_DOCS)
