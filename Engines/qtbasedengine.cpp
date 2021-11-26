#include "qtbasedengine.h"

#include <QString>
#include <QRegExp>
#include <QDirIterator>

QStringList QtBasedEngine::getFiles(const QString path, const QString pattern)
{
    QStringList files;

    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        auto cleanPath = QFileInfo(QDir::cleanPath(it.next()));

        if (cleanPath.isDir())
            continue;

        const auto &fileName = cleanPath.fileName();

        if (passPattern(pattern, fileName)) {
            files << cleanPath.absoluteFilePath();
        }
    }

    return files;
}

bool QtBasedEngine::passPattern(const QString pattern, const QString file)
{
    QRegExp rx(pattern);
    rx.setPatternSyntax(QRegExp::Wildcard);
    return rx.exactMatch(file);
}
