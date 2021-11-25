#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QListView;
class QTextEdit;
class QStringListModel;
class QLineEdit;

class ControlWidget : public QWidget
{
    Q_OBJECT

    QVBoxLayout *mainLayout {nullptr};
    QHBoxLayout *fileNameLayout {nullptr};

    QLineEdit *catalogName  {nullptr};
    //QLabel *catalogName  {nullptr};
    //QTextEdit *filesToFind {nullptr};
    QLineEdit *patternToFind  {nullptr};
    QPushButton *controlButton  {nullptr};

    QListView *filesView {nullptr};
    QStringListModel *listModel {nullptr};

    QPushButton *saveButton  {nullptr};

    QString currentCatalog;
    QStringList filesToShow;

    void disableControls();
    void enableControls();
public:
    explicit ControlWidget(QWidget *parent = nullptr);

    void onDirectoryChanged(const QString newDirectory);
    void onStartButton(bool checked);
    void onSaveButton(bool checked);

signals:

};

#endif // CONTROLWIDGET_H
