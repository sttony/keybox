
#include "CSyncEngine.h"
#include "CRequest.h"
using namespace std;

void CSyncEngine::Register(){
    // read url from kb file
    string sync_url = m_pKbfile->GetHeader().GetSyncUrl();
    // read email from kb file
    string sync_email = m_pKbfile->GetHeader().GetSyncEmail();
    // CRequest request;


}