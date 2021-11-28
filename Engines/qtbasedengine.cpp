#include "qtbasedengine.h"

#include <QString>
#include <QRegExp>
#include <QDirIterator>

QStringList QtBasedEngine::getFiles(const QString &path, const QString &pattern,  std::atomic_bool &toCancel)
{
    QStringList files;
    QStringList emptyFiles;
    long long counter = 0;
    QFileInfo cleanPath;
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        cleanPath = QFileInfo(QDir::cleanPath(it.next()));

        if (cleanPath.isDir())
            continue;

        if (passPattern(pattern, cleanPath.fileName())) {
            files << cleanPath.absoluteFilePath();
        }
        if (counter % 100000 == 0) {
            qDebug() << "counter " << counter;
            if (toCancel.load())
                return emptyFiles;
        }
        ++counter;
    }

    return files;
}

bool QtBasedEngine::passPattern(const QString &pattern, const QString &file)
{
    QRegExp rx(pattern);
    rx.setPatternSyntax(QRegExp::Wildcard);
    return rx.exactMatch(file);
}
