#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>
#include <QtConcurrent/QtConcurrent>

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
    QLineEdit *patternToFind  {nullptr};
    QPushButton *controlButton  {nullptr};
    QPushButton *cancelButton  {nullptr};

    QListView *filesView {nullptr};
    QStringListModel *listModel {nullptr};

    QPushButton *saveButton  {nullptr};

    QString currentCatalog;
    QStringList filesToShow;

    std::atomic_bool toCancel;
    QFutureWatcher<QStringList> watcherForConcurrentSearching;
    QFuture<QStringList> futureForConcurrentSearching;

    void disableControls();
    void enableControls();

    void onStartButton(bool checked);
    void onCancelButton(bool checked);
    void onSaveButton(bool checked);
    void handleFinishedSearchingTask();
    void handleCanceledSearchingTask();
public:
    explicit ControlWidget(QWidget *parent = nullptr);

    void onDirectoryChanged(const QString newDirectory);
};

#endif // CONTROLWIDGET_H
