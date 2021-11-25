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

#include <QtConcurrent/QtConcurrent>

ControlWidget::ControlWidget(QWidget *parent) :
    QWidget{parent},
    mainLayout {new QVBoxLayout(this)},
    fileNameLayout {new QHBoxLayout(this)},
    //catalogName {new QLabel(Helpers::catalogLabel, this)},
    //filesToFind {new QTextEdit(this)},
    catalogName {new QLineEdit(Helpers::catalogLabel, this)},
    patternToFind {new QLineEdit(this)},
    controlButton {new QPushButton(Helpers::buttonLabelToStart, this)},
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
    mainLayout->addLayout(fileNameLayout);


    //filesToShow << "Clair de Lune" << "Reverie" << "Prelude";
    filesToShow << Helpers::defaultInfoInSearchResult;
    listModel->setStringList(filesToShow);
    filesView->setEditTriggers(QAbstractItemView::NoEditTriggers);// TODO: ?
    filesView->setModel(listModel);

    mainLayout->addWidget(filesView);
    mainLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &ControlWidget::onSaveButton);
    connect(controlButton, &QPushButton::clicked, this, &ControlWidget::onStartButton);
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

    // clear model
    auto rowCountClean = listModel->rowCount();
    if (rowCountClean > 1) {
        listModel->removeRows(1, rowCountClean - 1);
    }

    // set async task to search
    auto initialPath = catalogName->text();
    auto indexOfRoot = catalogName->text().indexOf(QDir::separator());
    auto clearPath= initialPath.mid(indexOfRoot, initialPath.size() - 1);

    QFuture<QStringList> future = QtConcurrent::run(QtBasedEngine::getFiles, clearPath, patternToFind->text());
    future.waitForFinished();
    auto files = future.result();
    //qDebug() << files;

    //filesToShow = files;
    //filesToShow << "Clair de Lune" << "Reverie" << "Prelude";
    qDebug() << filesToShow;

//    auto rowCount = listModel->rowCount();          qDebug() << "rowCount" << rowCount;
//    QModelIndex modelIndex = listModel->index(rowCount - 1);  qDebug() << "modelIndex" << modelIndex;
//    //listModel->insertRows(rowCount, 1);
//    listModel->insertRow(rowCount);// insert row before index
//    listModel->setData(modelIndex, "99", Qt::DisplayRole);

//    filesView->setCurrentIndex(modelIndex);

    if (files.size() == 0) {
        auto rowCount = listModel->rowCount();          qDebug() << "rowCount" << rowCount;
        QModelIndex modelIndex = listModel->index(rowCount - 1);  qDebug() << "modelIndex" << modelIndex;
        listModel->insertRow(rowCount);// insert row before index
        listModel->setData(modelIndex, Helpers::defaultInfoInSearchResult, Qt::DisplayRole);

        filesView->setCurrentIndex(modelIndex);
    }

    auto rowCount = listModel->rowCount();          qDebug() << "rowCount" << rowCount;
    for(int i = 0; i < files.size(); ++i) {
        QModelIndex modelIndex = listModel->index(rowCount - 1);  qDebug() << "modelIndex" << modelIndex;
        //listModel->insertRows(rowCount, 1);
        listModel->insertRow(rowCount);// insert row before index
        listModel->setData(modelIndex, files[i], Qt::DisplayRole);

        filesView->setCurrentIndex(modelIndex);
        ++rowCount;
    }

    //emit listModel->dataChanged(modelIndex, modelIndex, {Qt::DisplayRole});


    enableControls();
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
            QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_6_2);
        std::for_each(filesToShow.cbegin(), filesToShow.cend(), [&out](const QString string){out << string;});// TODO: fix bug
        //out << list;
    }
}


void ControlWidget::disableControls()
{
    // TODO: implement method
}

void ControlWidget::enableControls()
{
    // TODO: implement method
}
