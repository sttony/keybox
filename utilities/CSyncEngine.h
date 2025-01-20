
#ifndef KEYBOX_SYNC_ENGINE_H
#define KEYBOX_SYNC_ENGINE_H
#include "CKBFile.h"

class CSyncEngine {
private:
    const CKBFile& m_kb_file;
public:
    CSyncEngine(const CKBFile& ckbFile): m_kb_file(ckbFile) {};
    ~CSyncEngine();

    void Register(); // Register 1st step
    void FinishRegister(); // Register 2nd step
    void Unregister();
    void Sync(); //sync;


};

#endif