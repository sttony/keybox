

#include "CRequest.h"

#include <utility>

#include "error_code.h"
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
    m_url = std::move(url);
    CURLcode res = curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
    if (res != CURLE_OK) {
        curl_easy_cleanup(m_curl);
        m_url.clear();
    }

    res = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, m_method.c_str());
    if (res != CURLE_OK) {
        curl_easy_cleanup(m_curl);
        m_url.clear();
    }
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
    CURLcode res;
    res = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    if (res != CURLE_OK) {
        return ERROR_CURL_ERROR_PREFIX | res;
    }
    res = curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
    if (res != CURLE_OK) {
        return ERROR_CURL_ERROR_PREFIX | res;
    }
    res = curl_easy_perform(m_curl);
    if (res != CURLE_OK) {
        return ERROR_CURL_ERROR_PREFIX | res;
    }
    return 0;
}

uint32_t CRequest::SetPayload(std::string &&payload) {
    m_payLoadBuff = payload;
    CURLcode res = curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_payLoadBuff.c_str());
    if (res != CURLE_OK) {
        return ERROR_CURL_ERROR_PREFIX | res;
    }
    return 0;
}

uint32_t CRequest::GetResponseCode() {
    long httpCode= -1;
    const CURLcode res =  curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if ( res == CURLE_OK ) {
        return httpCode;
    }
    return static_cast<uint32_t>(res);
}

const vector<unsigned char>& CRequest::GetResponsePayload() {
    return m_response_payload;
}

std::string CRequest::GetResponseHeader(std::string &&key_name) {
    return "";
}




