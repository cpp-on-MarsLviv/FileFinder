#include "mainwindow.h"
#include "catalogwidget.h"
#include "controlwidget.h"
#include "Helpers/constants.h"

#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createUI()
{
    mainSplitter = new QSplitter(Qt::Horizontal, this);

    catalogWidget = new CatalogWidget(this);
    controlWidget = new ControlWidget(this);

    mainSplitter->addWidget(catalogWidget);
    mainSplitter->addWidget(controlWidget);

    mainSplitter->setStretchFactor(0, Helpers::layoutProportions[0]);
    mainSplitter->setStretchFactor(1, Helpers::layoutProportions[1]);

    setCentralWidget(mainSplitter);

    connect(catalogWidget, &CatalogWidget::directryChanged, controlWidget, &ControlWidget::onDirectoryChanged);
}
