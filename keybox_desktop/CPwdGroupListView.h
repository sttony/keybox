

#ifndef KEYBOX_CPWDGROUPLISTVIEW_H
#define KEYBOX_CPWDGROUPLISTVIEW_H


#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QListView>

class CPwdGroupListView : public QListView{
    Q_OBJECT
public:
    explicit CPwdGroupListView(QWidget *parent = nullptr) : QListView(parent) {
        setAcceptDrops(true);
    }

signals:
    void entryDropped(int row, const QModelIndex& groupIndex);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasFormat("application/x-keybox-entry-row")) {
            event->acceptProposedAction();
        }
    }

    void dragMoveEvent(QDragMoveEvent *event) override {
        if (event->mimeData()->hasFormat("application/x-keybox-entry-row")) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasFormat("application/x-keybox-entry-row")) {
            QByteArray data = event->mimeData()->data("application/x-keybox-entry-row");
            int row = data.toInt();
            QModelIndex index = indexAt(event->pos());
            if (index.isValid()) {
                emit entryDropped(row, index);
                event->acceptProposedAction();
            }
        }
    }
};


#endif //KEYBOX_CPWDGROUPLISTVIEW_H
