#ifndef QTBASEDENGINE_H
#define QTBASEDENGINE_H

#include <QStringList>
#include <QSharedPointer>

class QString;

class QtBasedEngine
{
public:
    QtBasedEngine() = delete;

    static QSharedPointer<QStringList> getFiles(const QString &path, const QString &pattern, std::atomic_bool &toCancel);
    static bool passPattern(const QString &pattern, const QString &file);
};

#endif // QTBASEDENGINE_H
