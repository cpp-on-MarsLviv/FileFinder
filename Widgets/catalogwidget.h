#ifndef CATALOGWIDGET_H
#define CATALOGWIDGET_H

#include <QWidget>

class QFileSystemModel;
class QVBoxLayout;
class QTreeView;

class CatalogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogWidget(QWidget *parent = nullptr);

private:
    void onFileSystemViewClicked(const QModelIndex &index);

    QVBoxLayout *catalogLayout {nullptr};

    QTreeView *fileSystemView {nullptr};

    QFileSystemModel *fileSystemModel {nullptr};
signals:
    void directryChanged(const QString);
};

#endif // CATALOGWIDGET_H
