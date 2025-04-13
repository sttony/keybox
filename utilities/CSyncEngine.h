
#ifndef KEYBOX_SYNC_ENGINE_H
#define KEYBOX_SYNC_ENGINE_H
#include "CKBFile.h"

class CSyncEngine {
private:
    std::string m_sync_email;
    std::string m_sync_url;
public:
    CSyncEngine(const std::string& sync_email, const std::string& sync_url)
        : m_sync_email(sync_email), m_sync_url(sync_url) {};
    ~CSyncEngine();

    uint32_t Register(); // Register 1st step
    /**
     *  Register 2nd step
     * @return No register is pending
     *         register is not activate
     *         0, it is done.
     */
    uint32_t FinishRegister();

    void Unregister();
    uint32_t Retrieve();

    uint32_t Sync(CKBFile* pKBFile); //sync;


};

#endif