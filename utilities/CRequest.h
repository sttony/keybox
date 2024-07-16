

#ifndef KEYBOX_CPOSTREQUEST_H
#define KEYBOX_CPOSTREQUEST_H
#include <string>
#include <unordered_map>
#include <vector>
#include <curl/curl.h>

class CurlInitiator {
public:
    CurlInitiator();
};


class CRequest {
public:
    static const std::string GET;
    static const std::string POST;

    CRequest(std::string&& url, std::string method = GET);
    ~CRequest();
    void SetHeader(std::string&& header_name, std::string&& header_value);
    void SetPayload(std::string&& payload);
    uint32_t Send();
    uint32_t GetResponseCode();
    const std::vector<unsigned char>& GetResponsePayload();
    std::string GetResponseHeader(std::string&& key_name);

private:
    static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data);


    std::string m_method;
    CURL *m_curl;
    std::vector<unsigned char> m_response_payload;
    int m_response_code;
};


#endif //KEYBOX_CPOSTREQUEST_H
