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

#ifndef SETTINGSMIGRATION_H
#define SETTINGSMIGRATION_H

#include <QList>
#include <QString>

/**
 * @brief One-way, copy-only import of a previous product's configuration.
 *
 * RevivalPad uses its own configuration namespace so it can be installed
 * alongside AntiMicroX. This namespace holds the decision logic for the
 * first-run import, kept free of any UI so it can be tested directly.
 *
 * Guarantees enforced here, not by the caller:
 *   - An existing RevivalPad configuration is never overwritten.
 *   - Source files are only ever read. They are never moved, modified
 *     or deleted.
 */
namespace SettingsMigration {

/**
 * @brief A candidate configuration to import from.
 */
struct Source
{
    QString path;        /**< Absolute path of the source .ini. May be empty (skipped). */
    QString productName; /**< Human readable product name, shown to the user. */
};

enum class Status
{
    NotNeeded,     /**< A RevivalPad configuration already exists. Nothing was touched. */
    NoSourceFound, /**< Nothing to import. A fresh install. */
    Copied,        /**< A source was copied into the RevivalPad location. */
    Failed         /**< A source was found but the copy failed. */
};

struct Result
{
    Status status = Status::NoSourceFound;
    QString sourcePath;  /**< Populated for Copied and Failed. */
    QString productName; /**< Populated for Copied and Failed. */
};

/**
 * @brief The configuration locations RevivalPad imports from, newest first.
 */
QList<Source> defaultSources();

/**
 * @brief Import the newest available configuration into @p targetPath.
 *
 * @param targetPath  Destination .ini path in the RevivalPad namespace.
 * @param sources     Candidates in priority order; empty paths are skipped.
 */
Result migrate(const QString &targetPath, const QList<Source> &sources);

} // namespace SettingsMigration

#endif // SETTINGSMIGRATION_H
