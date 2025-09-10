
#include "CPwdEntry.h"
#include "CPwdGroup.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>


boost::property_tree::ptree CPwdEntry::toJsonObj() {
    boost::property_tree::ptree root;
    root.put("uuid", boost::uuids::to_string(m_uuid));
    root.put("title", m_title);
    root.put("user_name", m_username);
    root.put("url", m_url);
    root.put("group_id",  boost::uuids::to_string(m_group_uuid));
    root.add_child("note", m_note.toJsonObj());
    root.add_child("password", m_password.toJsonObj());
    root.add_child("attachment", m_attachment.toJsonObj());
    root.put("nano_timestamp", m_nano_timestamp);
    {
        boost::property_tree::ptree histArray;
        for (const auto& [blob, ts] : m_password_history) {
            boost::property_tree::ptree node;
            node.add_child("blob", blob.toJsonObj());
            node.put<long long>("timestamp", ts);
            histArray.push_back(std::make_pair("", node));
        }
        root.add_child("password_history", histArray);
    }


    return root;
}

const std::string &CPwdEntry::GetTitle() const {
    return m_title;
};

void CPwdEntry::SetTitle(const std::string &_title) {
    m_title = _title;
};


CPwdEntry::CPwdEntry() {
    m_uuid = boost::uuids::random_generator()();
    m_group_uuid = g_RootGroupId;
}

CPwdEntry::CPwdEntry(boost::uuids::uuid _uuid) {
    m_uuid = _uuid;
    m_group_uuid = g_RootGroupId;
}

uint32_t CPwdEntry::fromJsonObj(const boost::property_tree::ptree &jsonObj) {
    auto uuidStr = jsonObj.get<std::string>("uuid");
    m_uuid = boost::lexical_cast<boost::uuids::uuid>(uuidStr);
    m_title = jsonObj.get<std::string>("title");
    m_username = jsonObj.get<std::string>("user_name");
    m_url = jsonObj.get<std::string>("url");
    m_group_uuid = boost::uuids::string_generator()(jsonObj.get<std::string >("group_id"));

    boost::property_tree::ptree noteJsonObj = jsonObj.get_child("note");
    m_note.fromJsonObj(noteJsonObj);

    boost::property_tree::ptree passwordJsonObj = jsonObj.get_child("password");
    m_password.fromJsonObj(passwordJsonObj);

    boost::property_tree::ptree attachmentJsonObj = jsonObj.get_child("attachment");
    m_attachment.fromJsonObj(attachmentJsonObj);

    // Deserialize entry-level nano timestamp if present
    if (auto ts = jsonObj.get_optional<decltype(m_nano_timestamp)>("nano_timestamp")) {
        m_nano_timestamp = *ts;
    }

    // Deserialize password history if present
    if (auto histOpt = jsonObj.get_child_optional("password_history")) {
        m_password_history.clear();
        for (const auto& kv : *histOpt) {
            const auto& node = kv.second;

            CMaskedBlob blob;
            if (auto blobChild = node.get_child_optional("blob")) {
                blob.fromJsonObj(*blobChild);
            } else {
                // Fallback if the node itself is a CMaskedBlob object
                blob.fromJsonObj(node);
            }

            long long item_ts = node.get<long long>("timestamp", node.get<long long>("ts", 0));
            m_password_history.emplace_back(std::move(blob), item_ts);
        }
    }


    return 0;
}

boost::uuids::uuid CPwdEntry::GetGroup() const {
    return m_group_uuid;
}

void CPwdEntry::SetGroup(boost::uuids::uuid uuid) {
    m_group_uuid = uuid;
}

long long CPwdEntry::GetNanoTimestamp() const {
    return m_nano_timestamp;
}

void CPwdEntry::SetNanoTimestamp(long long ts) {
    m_nano_timestamp = ts;
}

const std::vector<std::pair<CMaskedBlob, long long>>& CPwdEntry::GetPasswordHistory() const {
    return m_password_history;
}

void CPwdEntry::SetPasswordHistory(const std::vector<std::pair<CMaskedBlob, long long>>& hist) {
    m_password_history = hist;
}

void CPwdEntry::SetPasswordHistory(std::vector<std::pair<CMaskedBlob, long long>>&& hist) {
    m_password_history = std::move(hist);
}