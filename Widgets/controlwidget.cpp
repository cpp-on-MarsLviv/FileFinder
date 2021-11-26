#include "controlwidget.h"
#include "Helpers/constants.h"
#include "Engines/qtbasedengine.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QListView>
#include <QStringListModel>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QDirIterator>

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
    saveButton {new QPushButton(Helpers::saveButtonLabel, this)}
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

    filesToShow << Helpers::defaultInfoInSearchResult;
    listModel->setStringList(filesToShow);
    filesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    filesView->setModel(listModel);

    mainLayout->addWidget(filesView);
    saveButton->setMinimumWidth(Helpers::saveButtonMinWidth);
    mainLayout->addWidget(saveButton, 1, Qt::AlignHCenter);

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
    qDebug() << "onStartButton()";
    disableControls();

    // clean model
    auto rowCountClean = listModel->rowCount();
    if (rowCountClean > 1) {
        listModel->removeRows(1, rowCountClean - 1);
    }

    // set async task to search
    connect(&watcherForConcurrentSearching, &QFutureWatcher<QStringList>::finished, this, &ControlWidget::handleFinishedSearchingTask);
    connect(&watcherForConcurrentSearching, &QFutureWatcher<QStringList>::canceled, this, &ControlWidget::handleCanceledSearchingTask);

    futureForConcurrentSearching = QtConcurrent::run(QtBasedEngine::getFiles, currentCatalog, patternToFind->text());
    watcherForConcurrentSearching.setFuture(futureForConcurrentSearching);
}

void ControlWidget::onCancelButton(bool /*checked*/)
{
    // TODO: implement method
}

void ControlWidget::onSaveButton(bool /*checked*/)
{
    qDebug() << "onSaveButton()";
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
        std::for_each(filesToShow.cbegin(), filesToShow.cend(), [&out](const QString string){

            out << string.toUtf8();
            out << "\n";
        });
    }
}

void ControlWidget::handleFinishedSearchingTask()
{
    qDebug() << "handleFinishedSearchingTask()";
    if (!futureForConcurrentSearching.isValid()) {
        qDebug() << "future not valid";
        return;
    }
    auto files = futureForConcurrentSearching.takeResult();

    filesToShow = files;

    if (files.size() == 0) {
        auto rowCount = listModel->rowCount();
        QModelIndex modelIndex = listModel->index(rowCount - 1);
        listModel->insertRow(rowCount);
        listModel->setData(modelIndex, Helpers::defaultInfoInSearchResult, Qt::DisplayRole);

        filesView->setCurrentIndex(modelIndex);
    }

    auto rowCount = listModel->rowCount();
    for(int i = 0; i < files.size(); ++i) {
        QModelIndex modelIndex = listModel->index(rowCount - 1);
        listModel->insertRow(rowCount);                             // TODO: optimize, like listModel->insertRows(rowCount, 1);
        listModel->setData(modelIndex, files[i], Qt::DisplayRole);  // or using sliding window

        filesView->setCurrentIndex(modelIndex);
        ++rowCount;
    }

    enableControls();
}

void ControlWidget::handleCanceledSearchingTask()
{
    // TODO: implement method
    enableControls();
}


void ControlWidget::disableControls()
{
    // TODO: implement method
}

void ControlWidget::enableControls()
{
    // TODO: implement method
}
