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
    listModel {new QStringListModel(this)},
    saveButton {new QPushButton(Helpers::saveButtonLabel, this)},
    modelFiles (QSharedPointer<QStringList>::create()),
    obtainedFiles (QSharedPointer<QStringList>::create())
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

    for (int i = 0; i < Helpers::listModelSize; ++i)
        *modelFiles << QString::number(i);

    listModel->setStringList(*modelFiles);
    filesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    filesView->setModel(listModel);

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
    toCancel.store(false);

    // clear Model
    modelFiles->clear();
    for (int i = 0; i < modelFiles->size(); ++i)
        *modelFiles << QString::number(i + 100);

    emit listModel->dataChanged(listModel->index(0, 0), listModel->index(listModel->rowCount() - 1, 0), {Qt::DisplayRole});

    connect(&watcherForConcurrentSearching, &QFutureWatcher<QStringList>::finished, this, &ControlWidget::handleFinishedSearchingTask);
    connect(&watcherForConcurrentSearching, &QFutureWatcher<QStringList>::canceled, this, &ControlWidget::handleCanceledSearchingTask);

    futureForConcurrentSearching = QtConcurrent::run(QtBasedEngine::getFiles, currentCatalog, patternToFind->text(), std::ref(toCancel));
    watcherForConcurrentSearching.setFuture(futureForConcurrentSearching);
}

void ControlWidget::onCancelButton(bool /*checked*/)
{
    toCancel.store(true);
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

    } else {
        // apply slicing window
    }

//    auto rowCount = listModel->rowCount();
//    for(int i = 0; i < obtainedFiles->size(); ++i) {
//        QModelIndex modelIndex = listModel->index(rowCount - 1);
//        listModel->insertRow(rowCount);
//        listModel->setData(modelIndex, obtainedFiles->at(i), Qt::DisplayRole);

//        filesView->setCurrentIndex(modelIndex);
//        ++rowCount;
//    }

    enableControls();
}

void ControlWidget::handleCanceledSearchingTask()
{
    futureForConcurrentSearching.waitForFinished();
    toCancel.store(false);
    enableControls();
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
