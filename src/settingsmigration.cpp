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

#include "settingsmigration.h"

#include "common.h"
#include "config.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace SettingsMigration {

QList<Source> defaultSources()
{
    return {
        {PadderCommon::configAntiMicroXFilePath(), QStringLiteral(REVIVALPAD_UPSTREAM_NAME)},
        {PadderCommon::configLegacyFilePath(), QStringLiteral("antimicroX")},
        {PadderCommon::configAntimicroLegacyFilePath(), QStringLiteral("antimicro")},
    };
}

Result migrate(const QString &targetPath, const QList<Source> &sources)
{
    Result result;

    const QFileInfo target(targetPath);
    if (target.exists() && target.isFile())
    {
        // Never overwrite an existing RevivalPad configuration.
        result.outcome = Outcome::NotNeeded;
        return result;
    }

    QFileInfo chosen;
    for (const Source &candidate : sources)
    {
        if (candidate.path.isEmpty())
            continue;

        const QFileInfo info(candidate.path);
        if (info.exists() && info.isFile())
        {
            chosen = info;
            result.productName = candidate.productName;
            break;
        }
    }

    if (result.productName.isEmpty())
    {
        result.outcome = Outcome::NoSourceFound;
        return result;
    }

    result.sourcePath = chosen.absoluteFilePath();

    const QString targetDir = target.absolutePath();
    QDir().mkpath(targetDir);

    // QFile::copy() refuses to overwrite, which is the behaviour we want; the
    // existence check above already established the target is absent.
    const bool copied = QFile::copy(result.sourcePath, targetPath);
    result.outcome = copied ? Outcome::Copied : Outcome::Failed;
    return result;
}

} // namespace SettingsMigration
