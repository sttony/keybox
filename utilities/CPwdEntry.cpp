
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

    return 0;
}

boost::uuids::uuid CPwdEntry::GetGroup() const {
    return m_group_uuid;
}

void CPwdEntry::SetGroup(boost::uuids::uuid uuid) {
    m_group_uuid = uuid;
}
