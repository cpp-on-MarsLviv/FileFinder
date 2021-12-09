#include "controlwidget.h"
#include "Helpers/constants.h"
#include "Engines/qtbasedengine.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QListView>
#include <QStringListModel>
#include <QStringList>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QWheelEvent>

ControlWidget::ControlWidget(QWidget *parent) :
    QWidget{parent},
    mainLayout {new QVBoxLayout(this)},
    controlsLayout {new QHBoxLayout(this)},
    catalogName {new QLineEdit(Helpers::catalogLabel, this)},
    patternToFind {new QLineEdit(Helpers::defaultSearchPuttern, this)},
    controlButton {new QPushButton(Helpers::buttonLabelToStart, this)},
    cancelButton {new QPushButton(Helpers::buttonLabelToCancel, this)},
    searchResult {new QLineEdit(Helpers::noFilesInSearchResult, this)},
    filesView {new QListView(this)},
    filesModel {new QStringListModel(this)},
    saveButton {new QPushButton(Helpers::saveButtonLabel, this)},
    obtainedFiles (QSharedPointer<QStringList>::create()),
    modelFiles (QSharedPointer<QStringList>::create())
{
    catalogName->setReadOnly(true);
    mainLayout->addWidget(catalogName);

    patternToFind->setPlaceholderText(Helpers::placeholderFiles);
    patternToFind->setToolTip(Helpers::toolTipForPattern);

    controlsLayout->addWidget(patternToFind);
    controlsLayout->addWidget(controlButton);
    controlsLayout->addWidget(cancelButton);
    controlsLayout->setSpacing(15);
    mainLayout->addLayout(controlsLayout);

    searchResult->setReadOnly(true);
    mainLayout->addWidget(searchResult);

    for (int i = 0; i < Helpers::listModelSize; ++i) {
        //*modelFiles << QString::number(i);// for debugging
        *modelFiles << QString("");
    }

    filesModel->setStringList(*modelFiles);
    filesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    filesView->setModel(filesModel);

    mainLayout->addWidget(filesView);
    saveButton->setMinimumWidth(Helpers::saveButtonMinWidth);
    mainLayout->addWidget(saveButton, 1, Qt::AlignHCenter);

    cancelButton->setDisabled(true);

    connect(saveButton, &QPushButton::clicked, this, &ControlWidget::onSaveButton);
    connect(controlButton, &QPushButton::clicked, this, &ControlWidget::onStartButton);
    connect(cancelButton, &QPushButton::clicked, this, &ControlWidget::onCancelButton);
}

void ControlWidget::onDirectoryChanged(const QString newCatalog)
{
    catalogName->setText(Helpers::catalogLabel + newCatalog);
    currentCatalog = newCatalog;
}

void ControlWidget::onStartButton(bool /*checked*/)
{
    disableControls();
    searchResult->setText(Helpers::searchingInSearchResult);
    toCancelTask.store(false);

    modelFiles->clear();
    for (int i = 0; i < Helpers::listModelSize; ++i) {
        //*modelFiles << QString::number(i + 100); // for debugging
        *modelFiles << QString("");
    }
    filesModel->setStringList(*modelFiles);
    // TODO: figure out why previous row works and one of next dont.
//    emit filesModel->dataChanged(filesModel->index(0, 0), filesModel->index(filesModel->rowCount() - 1, 0), {Qt::DisplayRole});
//    emit filesModel->dataChanged(filesModel->index(0, 0), filesModel->index(modelFiles->size() - 1, 0), {Qt::DisplayRole});

    connect(&watcherForConcurrentSearching, &QFutureWatcher<QStringList>::finished, this, &ControlWidget::handleFinishedSearchingTask);
    connect(&watcherForConcurrentSearching, &QFutureWatcher<QStringList>::canceled, this, &ControlWidget::handleCanceledSearchingTask);

    futureForConcurrentSearching = QtConcurrent::run(QtBasedEngine::getFiles, currentCatalog, patternToFind->text(), std::ref(toCancelTask));
    watcherForConcurrentSearching.setFuture(futureForConcurrentSearching);
}

void ControlWidget::onCancelButton(bool /*checked*/)
{
    toCancelTask.store(true);
}

void ControlWidget::onSaveButton(bool /*checked*/)
{
    QString fileName = QFileDialog::getSaveFileName(this,
            "Save Files", "",
            "Text files (*.txt)");
    if (fileName.isEmpty())
            return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, "Unable to open file",
            file.errorString());
            return;
        }
        QTextStream out(&file);
        std::for_each(obtainedFiles->cbegin(), obtainedFiles->cend(), [&out](const QString string){

            out << string.toUtf8();
            out << "\n";
        });
    }
}

void ControlWidget::handleFinishedSearchingTask()
{
    if (!futureForConcurrentSearching.isValid()) {
        qDebug() << "future not valid";
        return;
    }
    obtainedFiles = futureForConcurrentSearching.takeResult();

    if (obtainedFiles->size() == 0) {
        searchResult->setText(Helpers::noFilesInSearchResult);
    } else if (obtainedFiles->size() <= Helpers::listModelSize) {
        searchResult->setText(Helpers::foundFilesInSearchResult.arg(obtainedFiles->size()));
        modelFiles = obtainedFiles;

        filesModel->setStringList(*modelFiles);
        // TODO: figure out why previous row works and one of next dont.
    //    emit filesModel->dataChanged(filesModel->index(0, 0), filesModel->index(filesModel->rowCount() - 1, 0), {Qt::DisplayRole});
    //    emit filesModel->dataChanged(filesModel->index(0, 0), filesModel->index(modelFiles->size() - 1, 0), {Qt::DisplayRole});
    } else {                        // apply sliding window
        searchResult->setText(Helpers::foundFilesInSearchResult.arg(obtainedFiles->size()));

        slidingWindowLimits.first = obtainedFiles->cbegin();
        slidingWindowLimits.second = slidingWindowLimits.first + Helpers::listModelSize;
        modelFiles = QSharedPointer<QStringList>::create(slidingWindowLimits.first, slidingWindowLimits.second);

        filesModel->setStringList(*modelFiles);
        // TODO: same as prev. case
    }

    enableControls();
}

void ControlWidget::handleCanceledSearchingTask()
{
    futureForConcurrentSearching.waitForFinished();
    toCancelTask.store(false);
    enableControls();
}

void ControlWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() != Qt::Key_Enter)
        return;

    if (this->focusWidget() == controlButton) {
        onStartButton(true);
    } else if (this->focusWidget() == cancelButton) {
        onCancelButton(true);
    } else if (this->focusWidget() == saveButton) {
        onSaveButton(true);
    }
}

void ControlWidget::wheelEvent(QWheelEvent *event)
{
    if (obtainedFiles->size() <= Helpers::listModelSize) {
        event->accept();
        return;
    }

    QPoint numDegrees = event->angleDelta();
    auto delta = numDegrees.ry();
    int filesToScroll = std::abs(-delta / 30);

    if (delta < 0) {    // Scroll down
        auto nextSecond = std::min(obtainedFiles->cend(), slidingWindowLimits.second + filesToScroll);
        slidingWindowLimits.second = nextSecond;
        slidingWindowLimits.first = slidingWindowLimits.second - Helpers::listModelSize;

        modelFiles = QSharedPointer<QStringList>::create(slidingWindowLimits.first, slidingWindowLimits.second);
        filesModel->setStringList(*modelFiles);
        // TODO: same as prev.
    } else {        // Scroll up
        auto nextFirst = std::max(obtainedFiles->cbegin(), slidingWindowLimits.first - filesToScroll);
        slidingWindowLimits.first = nextFirst;
        slidingWindowLimits.second = slidingWindowLimits.first + Helpers::listModelSize;

        modelFiles = QSharedPointer<QStringList>::create(slidingWindowLimits.first, slidingWindowLimits.second);
        filesModel->setStringList(*modelFiles);
        // TODO: same as prev.
    }
    event->accept();
}

void ControlWidget::disableControls()
{
    controlButton->setDisabled(true);
    saveButton->setDisabled(true);
    patternToFind->setDisabled(true);

    cancelButton->setEnabled(true);
}

void ControlWidget::enableControls()
{
    controlButton->setEnabled(true);
    saveButton->setEnabled(true);
    patternToFind->setEnabled(true);

    cancelButton->setDisabled(true);
}
