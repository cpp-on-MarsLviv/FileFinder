#include "catalogwidget.h"
#include "Helpers/constants.h"

#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QTreeView>


CatalogWidget::CatalogWidget(QWidget *parent) :
    QWidget{parent},
    catalogLayout{new QVBoxLayout(this)},
    fileSystemView{new QTreeView{this}},
    fileSystemModel{new QFileSystemModel(this)}
{
    #if defined(Q_OS_LINUX)
    fileSystemModel->setRootPath(Helpers::emptyRootPathLinux);
    #elif defined(Q_OS_WIN)
    fileSystemModel->setRootPath(Helpers::emptyRootPathWin);
    #else
    fileSystemModel->setRootPath(Helpers::emptyRootPathLinux);
    #endif

    fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Drives);
    fileSystemView->setModel(fileSystemModel);

    catalogLayout->addWidget(fileSystemView);
    catalogLayout->setContentsMargins(Helpers::catalogWidgetMargins[0], Helpers::catalogWidgetMargins[1], Helpers::catalogWidgetMargins[2], Helpers::catalogWidgetMargins[3]);

    this->setLayout(catalogLayout);

    connect(fileSystemView, &QTreeView::clicked, this, &CatalogWidget::onFileSystemViewClicked);
    connect(fileSystemView, &QTreeView::expanded, this, &CatalogWidget::onFileSystemViewClicked);
}

void CatalogWidget::onFileSystemViewClicked(const QModelIndex &index)
{
    QString path = fileSystemModel->fileInfo(index).absoluteFilePath();
    emit directryChanged(path);
}

