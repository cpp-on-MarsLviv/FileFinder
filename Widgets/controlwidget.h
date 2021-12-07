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
    QHBoxLayout *controlsLayout {nullptr};

    QLineEdit *catalogName  {nullptr};
    QLineEdit *patternToFind  {nullptr};
    QPushButton *controlButton  {nullptr};
    QPushButton *cancelButton  {nullptr};

    QLineEdit *searchResult  {nullptr};

    QListView *filesView {nullptr};
    QStringListModel *listModel {nullptr};

    QPushButton *saveButton  {nullptr};

    QString currentCatalog;

    /**
     * @brief obtainedFiles conatain all found files
     */
    QSharedPointer<QStringList> obtainedFiles;
    /**
     * @brief modelFiles conatain just small portion (equal to Helpers::listModelSize constant) of obtained files.
     * Template known as sliding window.
     */
    QSharedPointer<QStringList> modelFiles;

    QPair<QList<QString>::const_iterator, QList<QString>::const_iterator> slidingWindowLimits;
                                                // TODO: create SlidingStringListModel : public QStringListModel
                                               // with built-in sliding window feature

    /**
     * @brief wheelEvent
     * Moving sliding window (modelFiles) in bigger bunch (obtainedFiles).
     */
    void wheelEvent(QWheelEvent *);

    std::atomic_bool toCancelTask;
    QFutureWatcher<QSharedPointer<QStringList>> watcherForConcurrentSearching;
    QFuture<QSharedPointer<QStringList>> futureForConcurrentSearching;

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
