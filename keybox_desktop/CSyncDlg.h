
#ifndef CSYNCDLG_H
#define CSYNCDLG_H
#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QMetaType>
#include <atomic>
#include <thread>
#include <vector>


#include "CKBModel.h"

Q_DECLARE_METATYPE(std::vector<CPwdGroup>)
Q_DECLARE_METATYPE(std::vector<CPwdEntry>)

class CSyncDlg final : public QDialog {
Q_OBJECT
public:
    explicit CSyncDlg(CKBModel* pModel, QWidget *parent = nullptr);
    ~CSyncDlg() override;
private slots:
    void updateProgress(int value);
    void updateStatus(const QString& status);
    void startSync();
    void cancelSync();
    void syncThreadFunction();
    void applyRemoteData(std::vector<CPwdGroup> groups, std::vector<CPwdEntry> entries);


signals:
    void syncCompleted(bool success);
    void syncError(const QString& error);
    void remoteDataReady(std::vector<CPwdGroup> groups, std::vector<CPwdEntry> entries);

private:
    CKBModel* m_pModel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton *m_syncButton;
    QPushButton* m_cancelButton = nullptr;

    std::thread m_syncThread;
    std::atomic<bool> m_isSyncing{false};
};



#endif //CSYNCDLG_H
