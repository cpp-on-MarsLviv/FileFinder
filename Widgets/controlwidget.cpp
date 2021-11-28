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
    fileNameLayout {new QHBoxLayout(this)},
    catalogName {new QLineEdit(Helpers::catalogLabel, this)},
    patternToFind {new QLineEdit(Helpers::defaultSearchPuttern, this)},
    controlButton {new QPushButton(Helpers::buttonLabelToStart, this)},
    cancelButton {new QPushButton(Helpers::buttonLabelToCancel, this)},
    filesView {new QListView(this)},
    listModel {new QStringListModel(this)},
    saveButton {new QPushButton(Helpers::saveButtonLabel, this)},
    obtainedFiles (QSharedPointer<QStringList>::create())
{
    catalogName->setReadOnly(true);
    mainLayout->addWidget(catalogName);

    patternToFind->setPlaceholderText(Helpers::placeholderFiles);
    patternToFind->setToolTip(Helpers::toolTipForPattern);

    fileNameLayout->addWidget(patternToFind);
    fileNameLayout->addWidget(controlButton);
    fileNameLayout->addWidget(cancelButton);
    fileNameLayout->setSpacing(15);
    mainLayout->addLayout(fileNameLayout);

    *obtainedFiles << Helpers::defaultInfoInSearchResult;
    listModel->setStringList(*obtainedFiles);
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

    auto rowCountClean = listModel->rowCount();
    if (rowCountClean > 1) {
        listModel->removeRows(1, rowCountClean - 1);
    }

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
    auto files = futureForConcurrentSearching.takeResult();

    obtainedFiles = files;

    if (files->size() == 0) {
        auto rowCount = listModel->rowCount();
        QModelIndex modelIndex = listModel->index(rowCount - 1);
        listModel->insertRow(rowCount);
        listModel->setData(modelIndex, Helpers::defaultInfoInSearchResult, Qt::DisplayRole);

        filesView->setCurrentIndex(modelIndex);
    }

    auto rowCount = listModel->rowCount();
    for(int i = 0; i < files->size(); ++i) {
        QModelIndex modelIndex = listModel->index(rowCount - 1);
        listModel->insertRow(rowCount);
        listModel->setData(modelIndex, files->at(i), Qt::DisplayRole);

        filesView->setCurrentIndex(modelIndex);
        ++rowCount;
    }

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
