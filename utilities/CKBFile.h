//
// Created by tongl on 1/2/2024.
//

#ifndef KEYBOX_CKBFILE_H
#define KEYBOX_CKBFILE_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <functional>
#include <boost/uuid/random_generator.hpp>

#include "CAsymmetricKeyPair.h"
#include "CKBFileHeader.h"
#include "CipherEngine.h"
#include "CPwdEntry.h"
#include "CPwdGroup.h"

class CKBFile {
public:

    uint32_t Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t LoadHeader(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t LoadPayload(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t Lock(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t AddEntry(CPwdEntry _entry);
    CPwdEntry GetEntry(int idx);
    uint32_t SetEntry(const CPwdEntry& _entry);
    uint32_t RemoveEntry(boost::uuids::uuid _entryId);

    // TODO: for testing, expose them.
    const std::vector<CPwdEntry> &GetEntries() const;

    CKBFileHeader &GetHeader();

    void SetMasterKey(std::vector<unsigned char> key, std::vector<unsigned char>&& onepad);

    void SetMasterKey(CMaskedBlob p);

    CPwdEntry QueryEntryByTitle(const std::string &_title);

    uint32_t SetDerivativeParameters(const std::vector<unsigned char> &_salt, int num_round = 60000);

    const PBKDF2_256_PARAMETERS &GetDerivativeParameters() {
        return m_header.GetDerivativeParameters();
    }

    const std::vector<CPwdGroup>& GetGroups() const;
    uint32_t RemoveGroup(const std::string _uuid_str);
    uint32_t UpdateGroup(const std::string& uid, const std::string& name);

    uint32_t SetAsymKey(std::unique_ptr<CAsymmetricKeyPair> _key);


    // Sync related code
    // Retrieve from remote; if server responds with a JSON {"message": "..."}, it will be returned via outMessage.
    uint32_t RetrieveFromRemote(std::string& outMessage);
    // Backward-compatible wrapper that ignores the returned message.
    uint32_t RetrieveFromRemote();
    // Push to remote; returns server message if present.
    uint32_t PushToRemote(std::string& outMessage);
    // Backward-compatible wrapper.
    uint32_t PushToRemote();
    // Register; returns server message if present.
    uint32_t Register(std::string& outMessage);
    // Backward-compatible wrapper.
    uint32_t Register();
    // Setup a new client; returns encrypted URL and optional message.
    uint32_t SetupNewClient(std::vector<unsigned char>& outUrl, std::string& outMessage);
    // Backward-compatible wrapper.
    uint32_t SetupNewClient(std::vector<unsigned char>& outUrl);

private:
    CKBFileHeader m_header;
    std::vector<CPwdEntry> m_entries;
    std::vector<CPwdGroup> m_groups = {g_RootGroup};
    CMaskedBlob m_master_key;
    std::unique_ptr<CAsymmetricKeyPair> m_pAsymmetric_key_pair;

    boost::uuids::uuid m_client_uuid = boost::uuids::random_generator()();
};


#endif //KEYBOX_CKBFILE_H
