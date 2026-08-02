/* antimicrox Gamepad to KB+M event mapper
 * Copyright (C) 2015 Travis Nickles <nickles.travis@gmail.com>
 * Copyright (C) 2020 Jagoda Górska <juliagoda.pl@protonmail>
 * Copyright (C) 2020 Paweł Kotiuk <kotiuk@zohomail.eu>
 * Copyright (C) 2026 0x1-1 (RevivalPad fork modifications)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include "mousehelper.h"
#include "revivalpadsettings.h"

#include <QDir>
#include <QIcon>
#include <QThread>
#include <QTranslator>
#include <QWaitCondition>

#include <SDL2/SDL_version.h>

#ifdef Q_OS_WIN

/**
 * @brief Per-user configuration directory of an installed build, for an arbitrary product name.
 * @param productName lowercase product directory name, e.g. "revivalpad" or "antimicrox".
 */
static QString findWinSystemConfigPathFor(const QString &productName)
{
    QString temp;
    temp = (!qgetenv("LocalAppData").isEmpty()) ? QString::fromUtf8(qgetenv("LocalAppData")) + "\\" + productName
                                                : QDir::homePath() + "\\." + productName;
    return temp;
}

static QString findWinSystemConfigPath() { return findWinSystemConfigPathFor(QStringLiteral(REVIVALPAD_CONFIG_DIR_NAME)); }

static QString findWinLocalConfigPath()
{
    QString temp = QCoreApplication::applicationDirPath();
    return temp;
}

static QString findWinDefaultConfigPath()
{
    QString temp = findWinLocalConfigPath();
    QFileInfo dirInfo(temp);
    if (!dirInfo.isWritable())
    {
        temp = findWinSystemConfigPath();
    }

    return temp;
}

#endif

namespace PadderCommon {

/**
 * @brief Configuration directory for an arbitrary product name.
 *
 * RevivalPad uses its own configuration namespace so that it can be installed
 * and run alongside AntiMicroX. The same helper is reused to locate the
 * AntiMicroX / antimicroX / antimicro directories that first-run migration
 * imports from. See importLegacySettingsIfExist() in main.cpp.
 */
inline QString configPathFor(const QString &productName)
{
#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
    Q_UNUSED(productName)
    return findWinLocalConfigPath();
#elif defined(Q_OS_WIN)
    return findWinSystemConfigPathFor(productName);
#else
    return (!qgetenv("XDG_CONFIG_HOME").isEmpty()) ? QString::fromUtf8(qgetenv("XDG_CONFIG_HOME")) + "/" + productName
                                                   : QDir::homePath() + "/.config/" + productName;
#endif
}

inline QString configPath() { return configPathFor(QStringLiteral(REVIVALPAD_CONFIG_DIR_NAME)); }

const QString configFileName = QStringLiteral(REVIVALPAD_CONFIG_FILE_NAME);

inline QString joinConfigPath(const QString &dir, const QString &fileName)
{
#if defined(Q_OS_WIN)
    return QString(dir).append("\\").append(fileName);
#else
    return QString(dir).append("/").append(fileName);
#endif
}

inline QString configFilePath() { return joinConfigPath(configPath(), configFileName); }

/**
 * @brief Location of the AntiMicroX 3.x configuration this fork was based on.
 *
 * Kept for one-way, copy-only first-run migration. Never written to.
 */
inline QString configAntiMicroXFilePath()
{
#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
    // A portable build keeps its settings next to the executable, so there is
    // no shared per-user AntiMicroX location to import from.
    return QString();
#else
    return joinConfigPath(configPathFor(QStringLiteral("antimicrox")), QStringLiteral("antimicrox_settings.ini"));
#endif
}

/**
 * @brief Location of the older 'antimicroX' configuration (upstream legacy).
 */
inline QString configLegacyFilePath()
{
#if defined(Q_OS_WIN)
    return ""; // earlier vesrions of antimicrox was not supported by Windows
#else
    QString configPath = (!qgetenv("XDG_CONFIG_HOME").isEmpty())
                             ? QString::fromUtf8(qgetenv("XDG_CONFIG_HOME")) + "/antimicroX"
                             : QDir::homePath() + "/.config/antimicroX";

    return QString(configPath).append("/").append("antimicroX_settings.ini");
#endif
}

/**
 * @brief Location of the oldest 'antimicro' configuration (upstream legacy).
 */
inline QString configAntimicroLegacyFilePath()
{
#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
    return QString();
#elif defined(Q_OS_WIN)
    return joinConfigPath(configPathFor(QStringLiteral("antimicro")), QStringLiteral("antimicro_settings.ini"));
#else
    QString configPath = (!qgetenv("XDG_CONFIG_HOME").isEmpty())
                             ? QString::fromUtf8(qgetenv("XDG_CONFIG_HOME")) + "/antimicro"
                             : QDir::homePath() + "/.config/antimicro";
    return QString(configPath).append("/").append("antimicro_settings.ini");
#endif
}

const int LATESTCONFIGFILEVERSION = 19;
// Specify the last known profile version that requires a migration
// to be performed in order to be compatible with the latest version.
const int LATESTCONFIGMIGRATIONVERSION = 5;
// Distinct from AntiMicroX's socket key so both applications can run side by side.
const QString localSocketKey = QStringLiteral(REVIVALPAD_LOCAL_SOCKET_KEY);
const QString unhideCommand = "unhideWindow";
const QString programName = QStringLiteral(REVIVALPAD_DISPLAY_NAME);
const QString githubProjectPage = QStringLiteral(REVIVALPAD_HOMEPAGE);
const QString githubIssuesPage = QStringLiteral(REVIVALPAD_ISSUES_URL);
const QString wikiPage = QStringLiteral(REVIVALPAD_WIKI_URL);
const QString upstreamName = QStringLiteral(REVIVALPAD_UPSTREAM_NAME);
const QString upstreamBaseVersion = QStringLiteral(REVIVALPAD_UPSTREAM_BASE_VERSION);
const QString upstreamProjectPage = QStringLiteral(REVIVALPAD_UPSTREAM_URL);

const QString mouseDeviceName(REVIVALPAD_DISPLAY_NAME " Mouse Emulation");
const QString keyboardDeviceName(REVIVALPAD_DISPLAY_NAME " Keyboard Emulation");
const QString springMouseDeviceName(REVIVALPAD_DISPLAY_NAME " Abs Mouse Emulation");

const int REVIVALPAD_MAJOR_VERSION = PROJECT_MAJOR_VERSION;
const int REVIVALPAD_MINOR_VERSION = PROJECT_MINOR_VERSION;
const int REVIVALPAD_PATCH_VERSION = PROJECT_PATCH_VERSION;

const QString programVersion =
    QString("%1.%2.%3").arg(REVIVALPAD_MAJOR_VERSION).arg(REVIVALPAD_MINOR_VERSION).arg(REVIVALPAD_PATCH_VERSION)
#ifdef QT_DEBUG
    + "-d"
#endif
    ;
const QString sdlVersionUsed = ([] {
    SDL_version linkedver;
    SDL_GetVersion(&linkedver);
    return QString("%1.%2.%3").arg(linkedver.major).arg(linkedver.minor).arg(linkedver.patch);
})();

const QString sdlVersionCompiled = ([] {
    SDL_version compver;
    SDL_VERSION(&compver);
    return QString("%1.%2.%3").arg(compver.major).arg(compver.minor).arg(compver.patch);
})();

extern QWaitCondition waitThisOut;
extern QMutex sdlWaitMutex;
extern QMutex inputDaemonMutex;
extern bool editingBindings;
extern MouseHelper mouseHelperObj;

QString preferredProfileDir(RevivalPadSettings *settings);
QStringList arguments(const int &argc, char **argv);
QStringList parseArgumentsString(const QString &tempString);
void reloadTranslations(QTranslator *translator, QTranslator *appTranslator, const QString &language);
void lockInputDevices();
void unlockInputDevices();

/**
 * @brief Universal method for loading icons if current theme does not have this icon, then look for replacement in resources
 *
 * @param name - name of used icon like "document-open" according to spec
 * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @return QIcon
 */
QIcon loadIcon(QString name);

/**
 * @brief Universal method for loading icons
 *
 * @param name - name of used icon
 * @param fallback_location - location of icon loaded when icon described by name not found
 * @return QIcon
 */
QIcon loadIcon(const QString &name, const QString &fallback_location);

/*!
 * \brief Returns the "human-readable" name of the given profile.
 */
inline QString getProfileName(QFileInfo &profile)
{
    QString retVal = profile.completeBaseName();

    return retVal;
}

void log_system_config();
} // namespace PadderCommon

Q_DECLARE_METATYPE(QThread *)

#endif // COMMON_H
