#include "Widgets/mainwindow.h"
#include "Utils/applocation.h"
#include "Helpers/constants.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;

    Utils::AppLocation::calculateAppGeometry();
    mainWindow.move(Utils::AppLocation::appMoveOnX, Utils::AppLocation::appMoveOnY);       // move App to screen center
    mainWindow.setMinimumWidth(Utils::AppLocation::appMinWidth);
    mainWindow.setMinimumHeight(Utils::AppLocation::appMinHeight);

    mainWindow.setWindowTitle(Helpers::appName);

    mainWindow.show();
    return a.exec();
}
