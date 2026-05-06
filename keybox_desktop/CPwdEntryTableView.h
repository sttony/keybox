//
// Created by tongl on 3/22/2024.
//

#ifndef KEYBOX_CPWDENTRYTABLEVIEW_H
#define KEYBOX_CPWDENTRYTABLEVIEW_H


#include <QTableView>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

class CPwdEntryTableView : public QTableView {
    Q_OBJECT
public:
    explicit CPwdEntryTableView(QWidget *parent = nullptr) : QTableView(parent) {
        setDragEnabled(true);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragStartPosition = event->pos();
        }
        QTableView::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        if (!(event->buttons() & Qt::LeftButton))
            return;
        if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
            return;

        QModelIndex index = indexAt(m_dragStartPosition);
        if (index.isValid()) {
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;
            mimeData->setData("application/x-keybox-entry-row", QByteArray::number(index.row()));
            drag->setMimeData(mimeData);
            drag->exec(Qt::MoveAction);
        }
    }

private:
    void copySelectedEntryPassword() const;
    QPoint m_dragStartPosition;
};


#endif //KEYBOX_CPWDENTRYTABLEVIEW_H
