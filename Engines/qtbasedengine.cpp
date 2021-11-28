#include "qtbasedengine.h"

#include <QString>
#include <QRegExp>
#include <QDirIterator>

QSharedPointer<QStringList> QtBasedEngine::getFiles(const QString &path, const QString &pattern, std::atomic_bool &toCancel)
{
    QSharedPointer<QStringList> files = QSharedPointer<QStringList>::create();
    QSharedPointer<QStringList> emptyFiles = QSharedPointer<QStringList>::create();

    long long counter = 0;
    QFileInfo cleanPath;
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        cleanPath = QFileInfo(QDir::cleanPath(it.next()));

        if (cleanPath.isDir())
            continue;

        if (passPattern(pattern, cleanPath.fileName())) {
            *files << cleanPath.absoluteFilePath();
        }
        if (counter % 100000 == 0) {
            qDebug() << "files# " << counter;
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
