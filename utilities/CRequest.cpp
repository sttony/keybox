

#include "CRequest.h"

#include <utility>
using namespace std;

const string CRequest::GET = "GET";
const string CRequest::POST = "POST";

CurlInitiator g_curl;

CurlInitiator::CurlInitiator() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

CRequest::CRequest(string &&url, string method) {
    m_curl = curl_easy_init();
    m_method = std::move(method);
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, m_method.c_str());
}

CRequest::~CRequest(){
    if(m_curl != nullptr) {
        curl_easy_cleanup(m_curl);
    }
}

void CRequest::SetHeader(std::string &&header_name, std::string &&header_value) {


}

size_t CRequest::WriteCallback(void *ptr, size_t size, size_t nmemb, void *data) {
    CRequest* pRequest = reinterpret_cast<CRequest*>(data);
    pRequest->m_response_payload.insert(pRequest->m_response_payload.end(),
                                        reinterpret_cast<unsigned char*>(ptr),
                                        reinterpret_cast<unsigned char*>(ptr) + size * nmemb);
    return size*nmemb;
}

uint32_t CRequest::Send() {
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
    return curl_easy_perform(m_curl);
}

void CRequest::SetPayload(std::string &&payload) {

}

uint32_t CRequest::GetResponseCode() {
    long httpCode;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &httpCode);
    return httpCode;
}

const vector<unsigned char>& CRequest::GetResponsePayload() {
    return m_response_payload;
}

std::string CRequest::GetResponseHeader(std::string &&key_name) {
    return "";
}




