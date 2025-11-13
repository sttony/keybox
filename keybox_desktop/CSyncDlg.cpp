#include "CSyncDlg.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <thread>

#include "utilities/CRequest.h"
using namespace std;

CSyncDlg::CSyncDlg(CKBModel *pModel, QWidget *parent) {
    m_pModel = pModel;
    setWindowTitle("Synchronization");
    setFixedSize(400, 200);

    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);

    // Status label
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setText("Ready to sync");

    // Sync button
    m_syncButton = new QPushButton("Start Sync", this);
    connect(m_syncButton, &QPushButton::clicked, this, &CSyncDlg::startSync);

    m_cancelButton = new QPushButton("cancel Sync", this);
    connect(m_cancelButton, &QPushButton::clicked, this, &CSyncDlg::cancelSync);

    QHBoxLayout *btnLine = new QHBoxLayout();
    btnLine->addWidget(m_syncButton);
    btnLine->addWidget(m_cancelButton);

    // Add widgets to layout
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addLayout(btnLine);

    setLayout(mainLayout);

    // Connect sync signals
    connect(this, &CSyncDlg::syncCompleted, this, [this](bool success) {
        m_syncButton->setEnabled(true);
        if (success) {
            updateStatus("Synchronization completed successfully");
        }
    });

    connect(this, &CSyncDlg::syncError, this, [this](const QString& error) {
        m_syncButton->setEnabled(true);
        updateStatus("Sync error: " + error);
        QMessageBox::critical(this, "Sync Error", error);
    });


}

CSyncDlg::~CSyncDlg() {
    if (m_syncThread.joinable()) {
        m_syncThread.join();
    }
}

void CSyncDlg::startSync() {
    if (m_isSyncing) {
        return;
    }

    m_syncButton->setEnabled(false);
    m_progressBar->setValue(0);
    updateStatus("Starting synchronization...");
    if (m_syncThread.joinable()) {
        m_syncThread.join();
    }

    m_isSyncing = true;
    m_syncThread = std::thread(&CSyncDlg::syncThreadFunction, this);
}

void CSyncDlg::cancelSync() {
    m_isSyncing = false;
    if (m_syncThread.joinable()) {
        m_syncThread.join();
        m_syncButton->setEnabled(true);
        m_progressBar->setValue(0);
        updateStatus("Ready to sync");
    }

}

void CSyncDlg::syncThreadFunction() {
    try {
        updateProgress(10);
        updateStatus("Retrieving data from remote server...");

        std::string retrieveMsg;
        uint32_t result = m_pModel->RetrieveFromRemote(retrieveMsg);
        if (result != 0) {
            QString err = QString("Failed to retrieve remote data (error code: %1)").arg(result);
            if (!retrieveMsg.empty()) {
                err += QString("\nServer message: %1").arg(QString::fromStdString(retrieveMsg));
            }
            emit syncError(err);
            m_isSyncing = false;
            return;
        }
        updateProgress(50);

        updateStatus("Pushing data to remote server...");
        std::string pushMsg;
        result = m_pModel->PushToRemote(pushMsg);
        if (result != 0) {
            QString err = QString("Failed to pushing remote data (error code: %1)").arg(result);
            if (!pushMsg.empty()) {
                err += QString("\nServer message: %1").arg(QString::fromStdString(pushMsg));
            }
            emit syncError(err);
            m_isSyncing = false;
            return;
        }

        updateProgress(100);
        emit syncCompleted(true);
    } catch (const std::exception& e) {
        emit syncError(QString("Exception during sync: %1").arg(e.what()));
    }

    m_isSyncing = false;
}


void CSyncDlg::updateProgress(int value) {
    // Use QMetaObject::invokeMethod to safely update UI from another thread
    QMetaObject::invokeMethod(m_progressBar, "setValue",
                            Qt::QueuedConnection,
                            Q_ARG(int, value));

}

void CSyncDlg::updateStatus(const QString &status) {
    // Use QMetaObject::invokeMethod to safely update UI from another thread
    QMetaObject::invokeMethod(m_statusLabel, "setText",
                            Qt::QueuedConnection,
                            Q_ARG(QString, status));

}
