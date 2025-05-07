
#ifndef CSYNCDLG_H
#define CSYNCDLG_H
#include <QDialog>
#include <QProgressBar>
#include <QLabel>


#include "CKBModel.h"


class CSyncDlg final : public QDialog {
Q_OBJECT
public:
    explicit CSyncDlg(CKBModel* pModel, QWidget *parent = nullptr);
private slots:
    void updateProgress(int value);
    void updateStatus(const QString& status);

private:
    CKBModel* m_pModel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton* m_syncBtn = nullptr;
    QPushButton* m_cancelBtn = nullptr;
};



#endif //CSYNCDLG_H
