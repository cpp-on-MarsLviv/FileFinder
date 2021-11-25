QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += concurrent
QT += core5compat   # for QRegExp

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Engines/qtbasedengine.cpp \
    Utils/applocation.cpp \
    Widgets/catalogwidget.cpp \
    Widgets/controlwidget.cpp \
    Widgets/mainwindow.cpp \
    main.cpp \

HEADERS += \
    Engines/qtbasedengine.h \
    Helpers/constants.h \
    Utils/applocation.h \
    Widgets/catalogwidget.h \
    Widgets/controlwidget.h \
    Widgets/mainwindow.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
