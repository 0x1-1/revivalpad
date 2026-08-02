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
 * Covers the first-run import of an AntiMicroX configuration.
 *
 * The two properties that matter most for user trust are asserted directly:
 * an existing RevivalPad configuration is never overwritten, and the source
 * files are never modified or removed.
 */

#include "settingsmigration.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QtTest/QtTest>

class TestSettingsMigration : public QObject
{
    Q_OBJECT

  private:
    QTemporaryDir *tmp = nullptr;

    QString write(const QString &relativePath, const QString &contents)
    {
        const QString full = tmp->filePath(relativePath);
        QDir().mkpath(QFileInfo(full).absolutePath());
        QFile f(full);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
            return QString();
        QTextStream out(&f);
        out << contents;
        f.close();
        return full;
    }

    static QString read(const QString &path)
    {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
            return QString();
        return QString::fromUtf8(f.readAll());
    }

    QString target() const { return tmp->filePath("revivalpad/revivalpad_settings.ini"); }

  private slots:
    void init()
    {
        tmp = new QTemporaryDir();
        QVERIFY(tmp->isValid());
    }

    void cleanup()
    {
        delete tmp;
        tmp = nullptr;
    }

    // ---- Nothing to do --------------------------------------------------

    void noSourcesLeavesFreshInstallAlone()
    {
        const auto result = SettingsMigration::migrate(target(), {});
        QCOMPARE(result.status, SettingsMigration::Status::NoSourceFound);
        QVERIFY(!QFile::exists(target()));
    }

    void emptySourcePathsAreSkipped()
    {
        // configLegacyFilePath() returns an empty string on Windows; an empty
        // candidate must be ignored rather than treated as a real file.
        const auto result = SettingsMigration::migrate(target(), {{QString(), QStringLiteral("antimicroX")}});
        QCOMPARE(result.status, SettingsMigration::Status::NoSourceFound);
    }

    // ---- Import ---------------------------------------------------------

    void importsFromAntiMicroX()
    {
        const QString source = write("antimicrox/antimicrox_settings.ini", "[General]\nLanguage=de\n");
        QVERIFY(!source.isEmpty());

        const auto result = SettingsMigration::migrate(target(), {{source, QStringLiteral("AntiMicroX")}});

        QCOMPARE(result.status, SettingsMigration::Status::Copied);
        QCOMPARE(result.productName, QStringLiteral("AntiMicroX"));
        QVERIFY(QFile::exists(target()));
        QCOMPARE(read(target()), QStringLiteral("[General]\nLanguage=de\n"));
    }

    void createsTargetDirectoryWhenMissing()
    {
        const QString source = write("antimicrox/antimicrox_settings.ini", "[General]\n");
        const QString nested = tmp->filePath("deep/nested/revivalpad/revivalpad_settings.ini");
        QVERIFY(!QDir(QFileInfo(nested).absolutePath()).exists());

        const auto result = SettingsMigration::migrate(nested, {{source, QStringLiteral("AntiMicroX")}});

        QCOMPARE(result.status, SettingsMigration::Status::Copied);
        QVERIFY(QFile::exists(nested));
    }

    void originalFileIsNeverModifiedOrRemoved()
    {
        const QString contents = "[General]\nLanguage=pl\n";
        const QString source = write("antimicrox/antimicrox_settings.ini", contents);

        const auto result = SettingsMigration::migrate(target(), {{source, QStringLiteral("AntiMicroX")}});

        QCOMPARE(result.status, SettingsMigration::Status::Copied);
        QVERIFY2(QFile::exists(source), "the AntiMicroX configuration must not be moved or deleted");
        QCOMPARE(read(source), contents);
    }

    // ---- Precedence -----------------------------------------------------

    void prefersNewestAvailableSource()
    {
        // Fixture directories are deliberately not named "antimicrox"/"antimicroX":
        // those differ only in case, and on a case-insensitive filesystem (NTFS,
        // APFS) they would resolve to the same file and mask the ordering under
        // test. migrate() takes explicit paths, so the names here are arbitrary.
        const QString antimicrox = write("src_antimicrox/settings.ini", "newest\n");
        const QString antimicroX = write("src_antimicroX_legacy/settings.ini", "middle\n");
        const QString antimicro = write("src_antimicro_legacy/settings.ini", "oldest\n");

        const auto result = SettingsMigration::migrate(target(), {{antimicrox, QStringLiteral("AntiMicroX")},
                                                                 {antimicroX, QStringLiteral("antimicroX")},
                                                                 {antimicro, QStringLiteral("antimicro")}});

        QCOMPARE(result.status, SettingsMigration::Status::Copied);
        QCOMPARE(result.productName, QStringLiteral("AntiMicroX"));
        QCOMPARE(read(target()), QStringLiteral("newest\n"));
    }

    void fallsBackToOlderSourcesWhenNewerAreAbsent()
    {
        const QString missing = tmp->filePath("src_antimicrox/settings.ini");
        const QString antimicro = write("src_antimicro_legacy/settings.ini", "oldest\n");

        const auto result = SettingsMigration::migrate(
            target(), {{missing, QStringLiteral("AntiMicroX")}, {antimicro, QStringLiteral("antimicro")}});

        QCOMPARE(result.status, SettingsMigration::Status::Copied);
        QCOMPARE(result.productName, QStringLiteral("antimicro"));
        QCOMPARE(read(target()), QStringLiteral("oldest\n"));
    }

    // ---- Never overwrite ------------------------------------------------

    void existingRevivalPadConfigIsNeverOverwritten()
    {
        write("revivalpad/revivalpad_settings.ini", "MINE\n");
        const QString source = write("antimicrox/antimicrox_settings.ini", "THEIRS\n");

        const auto result = SettingsMigration::migrate(target(), {{source, QStringLiteral("AntiMicroX")}});

        QCOMPARE(result.status, SettingsMigration::Status::NotNeeded);
        QCOMPARE(read(target()), QStringLiteral("MINE\n"));
    }

    void repeatedMigrationIsIdempotent()
    {
        const QString source = write("antimicrox/antimicrox_settings.ini", "first\n");
        QCOMPARE(SettingsMigration::migrate(target(), {{source, QStringLiteral("AntiMicroX")}}).status,
                 SettingsMigration::Status::Copied);

        // The user edits their RevivalPad settings, then AntiMicroX settings change.
        write("revivalpad/revivalpad_settings.ini", "edited\n");
        write("antimicrox/antimicrox_settings.ini", "second\n");

        const auto again = SettingsMigration::migrate(target(), {{source, QStringLiteral("AntiMicroX")}});
        QCOMPARE(again.status, SettingsMigration::Status::NotNeeded);
        QCOMPARE(read(target()), QStringLiteral("edited\n"));
    }

    // ---- Defaults -------------------------------------------------------

    void defaultSourcesAreOrderedNewestFirst()
    {
        const auto sources = SettingsMigration::defaultSources();
        QCOMPARE(sources.size(), 3);
        QCOMPARE(sources.at(0).productName, QStringLiteral("AntiMicroX"));
        QCOMPARE(sources.at(1).productName, QStringLiteral("antimicroX"));
        QCOMPARE(sources.at(2).productName, QStringLiteral("antimicro"));
    }
};

QTEST_GUILESS_MAIN(TestSettingsMigration)
#include "testsettingsmigration.moc"
