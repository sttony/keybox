
#ifndef KEYBOX_SYNC_ENGINE_H
#define KEYBOX_SYNC_ENGINE_H
#include "CKBFile.h"

class CSyncEngine {
private:
    std::shared_ptr<CKBFile> m_pKbfile;
public:
    CSyncEngine(const CKBFile& ckbFile): m_pKbfile(std::make_shared<CKBFile>(ckbFile)) {};
    ~CSyncEngine();

    uint32_t Register(); // Register 1st step
    void FinishRegister(); // Register 2nd step
    void Unregister();
    void Sync(); //sync;


};

#endif