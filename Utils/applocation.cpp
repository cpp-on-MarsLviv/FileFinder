#include "applocation.h"

#include <QApplication>
#include <QScreen>

namespace Utils {

    int AppLocation::appMinWidth;
    int AppLocation::appMinHeight;
    int AppLocation::appMoveOnX;
    int AppLocation::appMoveOnY;

    void AppLocation::calculateAppGeometry()
    {
        QList <QScreen*> screens = QGuiApplication::screens();
        QRect  screenGeometry = (screens.size() == 1) ? screens[0]->geometry() : screens[1]->geometry();

        appMinWidth = std::min({appWidthDefault, screenGeometry.width()});
        appMinHeight = std::min({appHeightDefault, screenGeometry.height()});

        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        appMoveOnX = (screens.size() == 1) ? (screenWidth - appMinWidth) / 2 : (screenWidth - appMinWidth) / 2 + screenGeometry.width();
        appMoveOnY = (screenHeight - appMinHeight) / 2;
    }

}
