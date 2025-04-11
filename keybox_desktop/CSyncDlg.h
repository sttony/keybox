
#ifndef CSYNCDLG_H
#define CSYNCDLG_H
#include <QtWidgets/qdialog.h>

#include "CKBModel.h"
#include "utilities/CSyncEngine.h"


class SyncWorker : public QObject {
    Q_OBJECT

public:
    explicit SyncWorker(CSyncEngine *pSyncEngine, CKBModel *pModel, QObject *parent = nullptr)
        : QObject(parent), m_pSyncEngine(pSyncEngine), m_pModel(pModel) {
    }

private:
    CSyncEngine *m_pSyncEngine;
    CKBModel *m_pModel;

    signals:
        void valueChanged(int newValue);

    public slots:
        void doWork();
};


class CSyncDlg final : public QDialog {
Q_OBJECT
public:
    explicit CSyncDlg(CKBModel* pModel, QWidget *parent = nullptr);
private slots:


private:
    QPushButton* m_syncBtn = nullptr;
    QPushButton* m_cancelBtn = nullptr;
};



#endif //CSYNCDLG_H
