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
 * The rebrand must not break existing AntiMicroX profiles. These tests pin the
 * on-disk profile contract - format version constants, the .amgp extension and
 * the application/x-amgp MIME type - and check that a profile written by
 * AntiMicroX 3.6.1 still parses under RevivalPad's expectations.
 *
 * The MIME and desktop templates are read from the source tree so that a
 * rebrand that accidentally changed the media type or glob is caught here.
 */

#include "common.h"
#include "config.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QtTest/QtTest>

// Injected by CMake so the tests can read the metadata templates.
#ifndef REVIVALPAD_SOURCE_DIR
    #define REVIVALPAD_SOURCE_DIR "."
#endif

namespace {
// A minimal but representative profile as written by AntiMicroX 3.6.1.
const char *const kAntiMicroXProfile = R"(<?xml version="1.0" encoding="UTF-8"?>
<gamecontroller configversion="19" appversion="3.6.1">
    <sets>
        <set index="1">
            <button index="1">
                <slots>
                    <slot>
                        <code>0x1000030</code>
                        <mode>keyboard</mode>
                    </slot>
                </slots>
            </button>
        </set>
    </sets>
</gamecontroller>
)";

QString readSourceFile(const QString &relative)
{
    QFile f(QStringLiteral(REVIVALPAD_SOURCE_DIR) + QLatin1Char('/') + relative);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(f.readAll());
}
} // namespace

class TestProfileCompatibility : public QObject
{
    Q_OBJECT

  private slots:
    // ---- Format contract ------------------------------------------------

    void profileFormatVersionIsUnchanged()
    {
        // Bumping this would silently invalidate existing AntiMicroX profiles.
        QCOMPARE(PadderCommon::LATESTCONFIGFILEVERSION, 19);
        QCOMPARE(PadderCommon::LATESTCONFIGMIGRATIONVERSION, 5);
    }

    void antiMicroXProfileStillParses()
    {
        QXmlStreamReader xml(QString::fromUtf8(kAntiMicroXProfile));
        QVERIFY(xml.readNextStartElement());
        QCOMPARE(xml.name().toString(), QStringLiteral("gamecontroller"));

        const int configVersion = xml.attributes().value(QStringLiteral("configversion")).toInt();
        QCOMPARE(configVersion, PadderCommon::LATESTCONFIGFILEVERSION);
        QVERIFY2(configVersion >= PadderCommon::LATESTCONFIGMIGRATIONVERSION,
                 "an AntiMicroX 3.6.1 profile must not require migration");

        // The upstream appversion attribute must be tolerated, not rejected.
        QCOMPARE(xml.attributes().value(QStringLiteral("appversion")).toString(), QStringLiteral("3.6.1"));

        while (!xml.atEnd())
            xml.readNext();
        QVERIFY2(!xml.hasError(), qPrintable(xml.errorString()));
    }

    // ---- Desktop integration contract -----------------------------------

    void mimeTemplateKeepsTheAmgpType()
    {
        const QString mime = readSourceFile(QStringLiteral("other/io.github._0x1_1.revivalpad.xml.in"));
        QVERIFY2(!mime.isEmpty(), "MIME package template not found");

        // Changing either of these would orphan every existing profile.
        QVERIFY2(mime.contains(QStringLiteral("application/x-amgp")), "the application/x-amgp media type must be kept");
        QVERIFY2(mime.contains(QStringLiteral("*.amgp")), "the *.amgp glob must be kept");
    }

    void desktopTemplateAssociatesAmgpAndUsesTheForkIdentity()
    {
        const QString desktop = readSourceFile(QStringLiteral("other/io.github._0x1_1.revivalpad.desktop.in"));
        QVERIFY2(!desktop.isEmpty(), "desktop entry template not found");

        QVERIFY(desktop.contains(QStringLiteral("MimeType=application/x-amgp;")));
        QVERIFY(desktop.contains(QStringLiteral("Icon=@REVIVALPAD_APP_ID@")));
        QVERIFY(desktop.contains(QStringLiteral("Exec=@REVIVALPAD_EXECUTABLE_NAME@")));
        QVERIFY2(!desktop.contains(QStringLiteral("antimicro"), Qt::CaseInsensitive),
                 "desktop entry must not carry AntiMicroX branding");
    }

    void dbusInterfaceUsesTheForkApplicationId()
    {
        const QString xml = readSourceFile(QStringLiteral("other/io.github._0x1_1.revivalpad.inputdevice.xml.in"));
        QVERIFY2(!xml.isEmpty(), "D-Bus interface template not found");

        QVERIFY(xml.contains(QStringLiteral("<interface name=\"@REVIVALPAD_APP_ID@.InputDevice\">")));
        // Object paths are intentionally unchanged from upstream.
        QVERIFY(xml.contains(QStringLiteral("/InputDevice")));
        // Only declarations matter here. The template's leading comment names the
        // upstream service on purpose, to document the breaking change.
        QVERIFY2(!xml.contains(QStringLiteral("name=\"io.github.antimicrox")),
                 "no node or interface may declare the upstream AntiMicroX name");
    }
};

QTEST_GUILESS_MAIN(TestProfileCompatibility)
#include "testprofilecompatibility.moc"
