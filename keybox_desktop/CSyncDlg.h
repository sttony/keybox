
#ifndef CSYNCDLG_H
#define CSYNCDLG_H
#include <QtWidgets/qdialog.h>

#include "CKBModel.h"


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
