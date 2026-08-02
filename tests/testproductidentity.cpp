/* antimicrox Gamepad to KB+M event mapper
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

/*
 * Guards the RevivalPad product identity against silent regressions back to
 * AntiMicroX values. These are deliberately strict: a rebrand that leaks the
 * upstream configuration directory, socket key, application id or update
 * endpoint would let RevivalPad collide with an installed AntiMicroX, or offer
 * AntiMicroX releases as RevivalPad updates.
 */

#include "common.h"
#include "config.h"

#include <QtTest/QtTest>

class TestProductIdentity : public QObject
{
    Q_OBJECT

  private slots:
    // ---- Product naming -------------------------------------------------

    void displayNameIsRevivalPad()
    {
        QCOMPARE(QString(REVIVALPAD_DISPLAY_NAME), QStringLiteral("RevivalPad"));
        QCOMPARE(QString(REVIVALPAD_LOWERCASE_NAME), QStringLiteral("revivalpad"));
        QCOMPARE(QString(REVIVALPAD_EXECUTABLE_NAME), QStringLiteral("revivalpad"));
        QCOMPARE(QString(REVIVALPAD_PRODUCT_FAMILY), QStringLiteral("Revival"));
    }

    void programNameAndVersionRender()
    {
        QCOMPARE(PadderCommon::programName, QStringLiteral("RevivalPad"));

        // programVersion gains a "-d" suffix in debug builds; compare the numeric head.
        const QString expected =
            QStringLiteral("%1.%2.%3").arg(PROJECT_MAJOR_VERSION).arg(PROJECT_MINOR_VERSION).arg(PROJECT_PATCH_VERSION);
        QVERIFY2(
            PadderCommon::programVersion.startsWith(expected),
            qPrintable(
                QStringLiteral("programVersion '%1' does not start with '%2'").arg(PadderCommon::programVersion, expected)));
        QVERIFY(PROJECT_MAJOR_VERSION >= 1);
    }

    void upstreamAttributionIsRecorded()
    {
        // The fork must keep saying what it is based on - never erase this.
        QCOMPARE(PadderCommon::upstreamName, QStringLiteral("AntiMicroX"));
        QCOMPARE(PadderCommon::upstreamBaseVersion, QStringLiteral("3.6.1"));
        QVERIFY(PadderCommon::upstreamProjectPage.contains(QStringLiteral("AntiMicroX/antimicrox")));
    }

    // ---- Configuration namespace ---------------------------------------

    void configDirectoryIsRevivalPadNamespace()
    {
        const QString path = PadderCommon::configPath();
        QVERIFY2(path.contains(QStringLiteral("revivalpad"), Qt::CaseInsensitive),
                 qPrintable(QStringLiteral("config path '%1' is not in the revivalpad namespace").arg(path)));
        QVERIFY2(!path.contains(QStringLiteral("antimicro"), Qt::CaseInsensitive),
                 qPrintable(QStringLiteral("config path '%1' still refers to antimicro").arg(path)));
    }

    void configFileNameIsRevivalPadSettingsIni()
    {
        QCOMPARE(PadderCommon::configFileName, QStringLiteral("revivalpad_settings.ini"));
        QCOMPARE(QString(REVIVALPAD_CONFIG_FILE_NAME), QStringLiteral("revivalpad_settings.ini"));
        QVERIFY(PadderCommon::configFilePath().endsWith(QStringLiteral("revivalpad_settings.ini")));
    }

    void configFilePathLivesInsideConfigPath()
    {
        QVERIFY(PadderCommon::configFilePath().startsWith(PadderCommon::configPath()));
    }

    // ---- Runtime identity ----------------------------------------------

    void localSocketKeyIsDistinctFromAntiMicroX()
    {
        // Sharing this key would make RevivalPad and AntiMicroX treat each
        // other as a second instance of themselves.
        QCOMPARE(PadderCommon::localSocketKey, QStringLiteral("revivalpadSignalListener"));
        QVERIFY(PadderCommon::localSocketKey != QStringLiteral("antimicroxSignalListener"));
    }

    void applicationIdIsForkOwned()
    {
        const QString appId = QStringLiteral(REVIVALPAD_APP_ID);
        QVERIFY2(appId.startsWith(QStringLiteral("io.github.")), qPrintable(appId));
        QVERIFY2(appId.endsWith(QStringLiteral(".revivalpad")), qPrintable(appId));
        QVERIFY2(appId != QStringLiteral("io.github.antimicrox.antimicrox"),
                 "application id must not remain the upstream AntiMicroX id");
        QVERIFY2(!appId.contains(QStringLiteral("antimicro")), qPrintable(appId));

        // D-Bus name and desktop file name are derived from the same id, so the
        // three can never drift apart.
        QCOMPARE(QString(REVIVALPAD_DBUS_SERVICE), appId);
        QCOMPARE(QString(REVIVALPAD_DESKTOP_FILE_NAME), appId);
    }

    void virtualDeviceNamesAreRebranded()
    {
        QVERIFY(PadderCommon::mouseDeviceName.startsWith(QStringLiteral("RevivalPad")));
        QVERIFY(PadderCommon::keyboardDeviceName.startsWith(QStringLiteral("RevivalPad")));
        QVERIFY(PadderCommon::springMouseDeviceName.startsWith(QStringLiteral("RevivalPad")));
    }

    // ---- Update checker -------------------------------------------------

    void updateEndpointTargetsTheFork()
    {
        const QString api = QStringLiteral(REVIVALPAD_UPDATE_API_URL);
        QVERIFY2(api.startsWith(QStringLiteral("https://api.github.com/repos/")), qPrintable(api));
        QVERIFY2(api.endsWith(QStringLiteral("/releases/latest")), qPrintable(api));
        QVERIFY2(api.contains(QStringLiteral("/revivalpad/releases/latest")), qPrintable(api));

        // An AntiMicroX release must never be presented as a RevivalPad update.
        QVERIFY2(!api.contains(QStringLiteral("antimicro"), Qt::CaseInsensitive),
                 "update endpoint must not query the AntiMicroX repository");
        QVERIFY2(!QString(REVIVALPAD_LATEST_RELEASE_URL).contains(QStringLiteral("antimicro"), Qt::CaseInsensitive),
                 "release page link must not point at AntiMicroX");
    }

    void projectLinksTargetTheFork()
    {
        QVERIFY(!PadderCommon::githubProjectPage.contains(QStringLiteral("antimicro"), Qt::CaseInsensitive));
        QVERIFY(!PadderCommon::githubIssuesPage.contains(QStringLiteral("antimicro"), Qt::CaseInsensitive));
        QVERIFY(PadderCommon::githubIssuesPage.endsWith(QStringLiteral("/issues")));
        QVERIFY(PadderCommon::wikiPage.endsWith(QStringLiteral("/wiki")));
    }

    // ---- Translations ---------------------------------------------------

    void translationNamespaceIsRebranded()
    {
        QCOMPARE(QString(REVIVALPAD_TRANSLATION_DIR_NAME), QStringLiteral("revivalpad"));
        QCOMPARE(QString(REVIVALPAD_TRANSLATION_PREFIX), QStringLiteral("revivalpad_"));
    }
};

QTEST_GUILESS_MAIN(TestProductIdentity)
#include "testproductidentity.moc"
