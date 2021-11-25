#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

class QSplitter;
class CatalogWidget;
class ControlWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QSplitter *mainSplitter {nullptr};

    CatalogWidget *catalogWidget {nullptr};
    //CatalogWidget *catalogWidget1 {nullptr};
    ControlWidget * controlWidget {nullptr};

    void createUI();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
