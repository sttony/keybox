//
// Created by tongl on 1/2/2024.
//

#include "CKBFile.h"

#include <boost/lexical_cast.hpp>

#include "error_code.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <memory>

#include "CipherEngine.h"
#include "Base64Coder.h"
#include "CRequest.h"
#include "InitGlobalRG.h"
#include "CMaskedBlob.h"

using namespace std;

CPwdEntry CKBFile::QueryEntryByTitle(const string &_title) {
    for (const auto &entry: m_entries) {
        if (entry.GetTitle() == _title) {
            return entry;
        }
    }
    return {};
}

const vector<CPwdEntry> &CKBFile::GetEntries() const {
    return m_entries;
}

uint32_t CKBFile::Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    if (m_master_key.size() == 0) {
        return ERROR_MASTER_KEY_INVALID;
    }
    uint32_t cbHeaderRealSize = 0;
    uint32_t result = this->LoadHeader(pBuffer, cbBufferSize, cbHeaderRealSize);
    if ( result ) {
        return result;
    }

    result = this->LoadPayload(pBuffer + cbHeaderRealSize, cbBufferSize - cbHeaderRealSize, cbRealSize);
    if ( result ) {
        return result;
    }
    cbRealSize += cbHeaderRealSize;
    return 0;
}

uint32_t CKBFile::Lock(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    uint32_t result = this->Serialize(pBuffer, cbBufferSize, cbRealSize);
    if (result != 0) {
        return result;
    }
    m_entries.clear();
    return 0;
}

CKBFileHeader &CKBFile::GetHeader() {
    return m_header;
}

uint32_t CKBFile::AddEntry(CPwdEntry _entry) {
    for (auto &entry: m_entries) {
        if (entry.GetID() == _entry.GetID()) {
            // Determine which entry is more recent
            const auto existingTs = entry.GetNanoTimestamp();
            const auto incomingTs = _entry.GetNanoTimestamp();
            const bool incomingIsNewer = incomingTs >= existingTs;

            // Snapshot histories before replacing anything
            auto existingHist = entry.GetPasswordHistory();
            auto incomingHist = _entry.GetPasswordHistory();

            // Merge histories and keep only the 10 most recent
            std::vector<std::pair<CMaskedBlob, long long>> merged;
            merged.reserve(existingHist.size() + incomingHist.size());
            merged.insert(merged.end(), existingHist.begin(), existingHist.end());
            merged.insert(merged.end(), incomingHist.begin(), incomingHist.end());

            std::stable_sort(merged.begin(), merged.end(),
                             [](const auto &a, const auto &b) {
                                 return a.second > b.second; // newer first
                             });

            if (merged.size() > 10) {
                merged.resize(10);
            }
            // Use the most recent entry as the base
            if (incomingIsNewer) {
                entry = _entry;
            }
            entry.SetPasswordHistory(std::move(merged));
            return 0;
        }
    }
    m_entries.push_back(std::move(_entry));
    return 0;
}

void CKBFile::SetMasterKey(std::vector<unsigned char> key, std::vector<unsigned char>&& onepad) {
    m_master_key.Set(key, std::move(onepad));
}

uint32_t CKBFile::SetDerivativeParameters(const vector<unsigned char> &_salt, int num_round) {
    return m_header.SetDerivativeParameters(_salt, num_round);
}

void CKBFile::SetMasterKey(CMaskedBlob p) {
    m_master_key = p;
}

uint32_t CKBFile::Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    cbRealSize = 0;
    boost::property_tree::ptree pay_load;
    // pwd entries
    boost::property_tree::ptree entries;
    for (auto &entry: m_entries) {
        entries.push_back({"", entry.toJsonObj()});
    }
    pay_load.add_child("entries", entries);

    // pwd groups
    boost::property_tree::ptree groups;
    for (auto &group: m_groups) {
        groups.push_back({"", group.toJsonObj()});
    }
    pay_load.add_child("groups", groups);

    if (m_pAsymmetric_key_pair != nullptr) {
        // Asym key
        pay_load.add_child("asymmetric_key", m_pAsymmetric_key_pair->toJsonObj());
    }

    // client ID
    boost::property_tree::ptree client_id;
    client_id.put("uuid", to_string(m_client_uuid));
    pay_load.add_child("client_id", client_id);

    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    string pay_load_json = oss.str();

    uint32_t uResult = 0;
    if(pBuffer != nullptr) {
        uResult = m_header.CalculateHMAC(m_master_key.ShowBin(),
                                         reinterpret_cast<const unsigned char *>(pay_load_json.data()),
                                         pay_load_json.size());
        if (uResult) {
            return uResult;
        }
    }

    uResult = m_header.Serialize(pBuffer, cbBufferSize, cbRealSize);
    if (uResult) {
        if (pBuffer == nullptr && uResult == ERROR_BUFFER_TOO_SMALL) {
            // continue
        } else {
            return uResult;
        }
    }

    vector<unsigned char> encrypted_buff;
    CCipherEngine cipherEngine;
    uResult = cipherEngine.AES256EnDecrypt(
            (unsigned char *) pay_load_json.data(),
            pay_load_json.size(),
            m_master_key.ShowBin(),
            m_header.GetIV(),
            CCipherEngine::AES_CHAIN_MODE_CBC,
            CCipherEngine::AES_PADDING_MODE_PKCS7,
            true,
            encrypted_buff
    );
    if (uResult) {
        return uResult;
    }
    if (cbBufferSize < cbRealSize + encrypted_buff.size()) {
        if (pBuffer == nullptr) {
            cbRealSize += encrypted_buff.size();
            cipherEngine.CleanString(pay_load_json);
        }
        return ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(pBuffer + cbRealSize, &encrypted_buff[0], encrypted_buff.size());
    cbRealSize += encrypted_buff.size();
    cipherEngine.CleanString(pay_load_json);

    // clean
    return 0;
}

uint32_t CKBFile::LoadHeader(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    cbRealSize = 0;
    uint32_t uResult = 0;
    uResult = m_header.Deserialize(pBuffer, cbBufferSize, cbRealSize);

    if (uResult) {
        if (pBuffer == nullptr && uResult == ERROR_BUFFER_TOO_SMALL) {
            // continue
        } else {
            return uResult;
        }
    }
    m_groups = {g_RootGroup};
    return 0;
}

uint32_t CKBFile::LoadPayload(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    CCipherEngine cipherEngine;
    vector<unsigned char> decrypted_buff;
    uint32_t uResult = 0;
    uResult = cipherEngine.AES256EnDecrypt(
            pBuffer,
            cbBufferSize,
            m_master_key.ShowBin(),
            m_header.GetIV(),
            CCipherEngine::AES_CHAIN_MODE_CBC,
            CCipherEngine::AES_PADDING_MODE_PKCS7,
            false,
            decrypted_buff
    );
    if (uResult) {
        return uResult;
    }

    // load json from decrypted_buff
    decrypted_buff.back() = '\0'; // append a zero
    string temp ( decrypted_buff.begin(), decrypted_buff.end() );
    if (size_t lastBrace = temp.find_last_of('}'); lastBrace != std::string::npos) {
        temp = temp.substr(0, lastBrace+1);
    }
    temp.push_back('\n'); // boost serialize to json, a '\n' is appended to end.
    boost::property_tree::ptree entries_tree;

    // calculate HMAC with temp
    vector<unsigned char> vHmacSignature;
    cipherEngine.HMAC_SHA256(m_master_key.ShowBin(), reinterpret_cast<const unsigned char *>(temp.c_str()), temp.size(), vHmacSignature);
    if(vHmacSignature != m_header.GetHMACSignature()){
        return ERROR_MASTER_KEY_INVALID;
    }

    try {
        stringstream ss(temp);
        boost::property_tree::read_json(ss, entries_tree);
    }
    catch (exception &e) {
        return ERROR_INVALID_JSON;
    }

    // Clear existing entries
    m_entries.clear();
    m_groups.clear();
    m_pAsymmetric_key_pair.reset();

    if (auto entries = entries_tree.get_child_optional("entries"); entries.has_value()) {
        for (const auto &kv: entries.get()) {
            CPwdEntry entry;
            entry.fromJsonObj(kv.second);
            this->AddEntry(entry);
        }
    }

    if (auto groups = entries_tree.get_child_optional("groups"); groups.has_value()) {
        for (const auto &kv:groups.get()) {
            CPwdGroup group("");
            group.fromJsonObj(kv.second);
            m_groups.push_back(std::move(group));
        }
    }
    if (auto asymmetric_key = entries_tree.get_child_optional("asymmetric_key"); asymmetric_key.has_value()) {
        m_pAsymmetric_key_pair = std::make_unique<CAsymmetricKeyPair>();
        m_pAsymmetric_key_pair->fromJsonObj(asymmetric_key.get());
    }

    if (auto client_id = entries_tree.get_child_optional("client_id"); client_id.has_value()) {
        auto uuidStr = entries_tree.get_child("client_id").get<std::string>("uuid");
        m_client_uuid = boost::lexical_cast<boost::uuids::uuid>(uuidStr);
    }
    UpdateGroup(g_RootGroup.GetID(), g_RootGroup.GetName());
    return 0;
}

CPwdEntry CKBFile::GetEntry(int idx) {
    if(idx >= m_entries.size()){
        return {};
    }
    return m_entries[idx];
}

uint32_t CKBFile::SetEntry(const CPwdEntry& _entry) {
    for(auto& pe: m_entries){
        if( pe.GetID() == _entry.GetID()){
            pe = _entry;
            return 0;
        }
    }
    return ERROR_ENTRY_NOT_FOUND;
}

uint32_t CKBFile::RemoveEntry(boost::uuids::uuid _entryId) {
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
        if (it->GetID() == _entryId) {
            m_entries.erase(it);
            return 0;
        }
    }
    return ERROR_ENTRY_NOT_FOUND;
}

const std::vector<CPwdGroup> &CKBFile::GetGroups() const {
    return m_groups;
}

uint32_t CKBFile::RemoveGroup(const std::string uuid_str) {
    auto it = find_if(m_groups.begin(), m_groups.end(),  [uuid_str] (const auto& g){
       return g.GetID() == uuid_str;
    });
    if( it == m_groups.end()){
        return ERROR_ENTRY_NOT_FOUND;
    }
    m_groups.erase(it);

    // update entry with _uuid to root;
    for(auto& entry : m_entries){
        if( boost::uuids::to_string(entry.GetGroup()) == uuid_str){
            entry.SetGroup(g_RootGroupId);
        }
    }

    return 0;
}

uint32_t CKBFile::UpdateGroup(const std::string &uuid_str, const std::string &name) {
    for (auto &group: m_groups) {
        if (group.GetID() == uuid_str) {
            group.SetName(name);
            return 0;
        }
    }

    try{
        CPwdGroup pwdG = CPwdGroup(name,  boost::uuids::string_generator()(uuid_str));
        m_groups.push_back(pwdG);
    }
    catch(runtime_error e){
        return ERROR_INVALID_PARAMETER;
    }


    return 0;
}

uint32_t CKBFile::SetAsymKey(unique_ptr<CAsymmetricKeyPair> _key) {
    m_pAsymmetric_key_pair.reset();
    m_pAsymmetric_key_pair = std::move(_key);
    return 0;
}



/**
 *  Retrieve from Remote,  Send email and client ID.
 *  Server return
 *      1. an encrypted session key with email's pubkey
 *      2. AES256 encrypted payload.
 *  Client:
 *      1. decrypt the session key with email's prvkey
 *      2. decrypt the payload with session key
 *      3. load the payload as a new CKBFile.
 *      4. Merge the remote CKBFile with local
 * @return
 */
uint32_t CKBFile::RetrieveFromRemote() {
    uint32_t uResult = 0;
    const string& sync_url = m_header.GetSyncUrl();
    const string& sync_email = m_header.GetSyncEmail();
    CRequest request(sync_url+"/" + "retrieve", CRequest::POST);
    boost::property_tree::ptree pay_load;

    pay_load.put("email", sync_email);
    pay_load.put("client_id", to_string(m_client_uuid));
    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    request.SetPayload(oss.str());
    request.Send();
    if (request.GetResponseCode() != 200) {
        return ERROR_HTTP_ERROR_PREFIX | request.GetResponseCode();
    }

    // load payload json
    std::istringstream iss(string(reinterpret_cast<const char *>(request.GetResponsePayload().data()),
        reinterpret_cast<const char *>(request.GetResponsePayload().data()) + request.GetResponsePayload().size()));
    boost::property_tree::ptree response;
    try {
        boost::property_tree::read_json(iss, response);
    }
    catch (exception &e) {
        return ERROR_INVALID_JSON;
    }

    string encrypted_file_content = response.get<std::string>("encrypted_file_content");
    if ( encrypted_file_content.empty()) {
        return 0;
    }

    vector<unsigned char> encrypted_session_key_bytes = hex_to_bytes(response.get<std::string>("encrypted_session_key"));
    vector<unsigned char> session_key_bytes_decrypted;
    m_pAsymmetric_key_pair->Decrypt(encrypted_session_key_bytes, session_key_bytes_decrypted);
    session_key_bytes_decrypted.resize(32);


    vector<unsigned char> encrypted_file_content_bytes = hex_to_bytes(response.get<std::string>("encrypted_file_content"));
    vector<unsigned char> iv = hex_to_bytes(response.get<std::string>("iv"));

    vector<unsigned char> decrypted_buff;
    CCipherEngine cipherEngine;

    uResult = cipherEngine.AES256EnDecrypt(
        encrypted_file_content_bytes.data(),
        encrypted_file_content_bytes.size(),
        session_key_bytes_decrypted,
        iv,
        CCipherEngine::AES_CHAIN_MODE_CBC,
        CCipherEngine::AES_PADDING_MODE_PKCS7,
        false,
        decrypted_buff);

    if (uResult) {
        return uResult;
    }

    CKBFile remote_ckb_file;
    size_t cbRealSize = 0;
    remote_ckb_file.SetMasterKey(this->m_master_key);
    uResult = remote_ckb_file.Deserialize(decrypted_buff.data(), decrypted_buff.size(), uResult);
    if (uResult) {
        return uResult;
    }

    for (const auto &grp  : remote_ckb_file.GetGroups()) {
        this->UpdateGroup(grp.GetID(), grp.GetName());
    }

    for (const auto &entry : remote_ckb_file.GetEntries()) {
        this->AddEntry(entry);
    }

    return 0;
}

/**
 *  serialize the file,
 *  Sign with prv key
 *  Send { signature:
 *         payload
 *         }
 *
 * @return
 */
uint32_t CKBFile::PushToRemote() {
    vector<unsigned char> buff;
    uint32_t cbRealsize = 0;
    uint32_t result = this->Serialize(nullptr, 0, cbRealsize);
    if (result != ERROR_BUFFER_TOO_SMALL) {
        return result;
    }
    buff.resize(cbRealsize);
    cbRealsize = 0;
    result = this->Serialize(buff.data(), buff.size(), cbRealsize);
    if (result) {
        return result;
    }
    Base64Coder base64_coder;
    string payload_string;
    base64_coder.Encode(buff.data(), buff.size(), payload_string);

    vector<unsigned char> signature;
    m_pAsymmetric_key_pair->Sign(buff, signature);
    string signature_string;

    base64_coder.Encode(signature.data(), signature.size(), signature_string);

    CRequest request(m_header.GetSyncUrl() + "/" + "upload", CRequest::POST);
    boost::property_tree::ptree pay_load;

    pay_load.put("email", m_header.GetSyncEmail());
    pay_load.put("signature", signature_string);
    pay_load.put("payload", payload_string);
    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    request.SetPayload(oss.str());
    auto private_key = m_pAsymmetric_key_pair->GetPrivateKey(vector<unsigned char>(m_pAsymmetric_key_pair->GetPrivateKeyLength()));
    string private_key_string = private_key.Show();
    request.Send();
    if (request.GetResponseCode() == 200) {
        return 0;
    }
    else {
        return request.GetResponseCode() + ERROR_HTTP_ERROR_PREFIX;
    }
}

/**
 * 1. generate asymm pair
 * 3. send pubkey, email to url
 *
 * @return
 */
uint32_t CKBFile::Register() {
    // 1. re generate asymmetric key
    m_pAsymmetric_key_pair = std::make_unique<CAsymmetricKeyPair>();

    uint32_t result = m_pAsymmetric_key_pair->ReGenerate();
    if (result) {
        return result;
    }

    vector<unsigned char> pubKeyPad = g_RG.GetNextBytes(m_pAsymmetric_key_pair->GetPublicKeyLength());
    CMaskedBlob maskedPubKey= m_pAsymmetric_key_pair->GetPublicKey(std::move(pubKeyPad));

    CMaskedBlob pubKey = m_pAsymmetric_key_pair->GetPublicKey( vector<unsigned char>(m_pAsymmetric_key_pair->GetPublicKeyLength()));
    CRequest request(m_header.GetSyncUrl()+"/" + "register", CRequest::POST);
    boost::property_tree::ptree pay_load;
    pay_load.put("pubKey", pubKey.Show());
    pay_load.put("email", m_header.GetSyncEmail());
    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    request.SetPayload(oss.str());
    result = request.Send();
    if ( result != 0 ) {
        return result;
    };

    if (request.GetResponseCode() == 200) {
        return 0;
    }
    return request.GetResponseCode() | ERROR_HTTP_ERROR_PREFIX;
}

/**
 * 1. send new_client api, with email
 * 2. get encrypted presign url back
 * @return
 */
uint32_t CKBFile::SetupNewClient(vector<unsigned char>& outEncryptedUrl) {
    uint32_t uResult = 0;
    const string& sync_url = m_header.GetSyncUrl();
    const string& sync_email = m_header.GetSyncEmail();
    CRequest request(sync_url+"/" + "setup_new_client", CRequest::POST);
    boost::property_tree::ptree pay_load;

    pay_load.put("email", sync_email);
    pay_load.put("client_id", to_string(m_client_uuid));
    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    request.SetPayload(oss.str());
    request.Send();
    if (request.GetResponseCode() != 200) {
        return ERROR_HTTP_ERROR_PREFIX | request.GetResponseCode();
    }

    std::istringstream iss(string(reinterpret_cast<const char *>(request.GetResponsePayload().data()),
       reinterpret_cast<const char *>(request.GetResponsePayload().data()) + request.GetResponsePayload().size()) );
    boost::property_tree::ptree response;
    try {
        boost::property_tree::read_json(iss, response);
    }
    catch (exception &e) {
        return ERROR_INVALID_JSON;
    }
    outEncryptedUrl = hex_to_bytes(response.get<std::string>("file_url"));
    return 0;
}