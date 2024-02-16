//
// Created by tongl on 2/13/2024.
//

#include "CPwdEntry.h"
#include <boost/uuid/uuid_generators.hpp>


boost::property_tree::ptree CPwdEntry::toJsonObj() {
    boost::property_tree::ptree root;

    root.put("user_name", m_username);
    root.put("url", m_url);
    root.add_child("note", m_note.toJsonObj());
    root.add_child("password", m_password.toJsonObj());
    root.add_child("attachment", m_attachment.toJsonObj());

    return root;
}
const std::string& CPwdEntry::GetTitle() {
    return m_title;
};

void CPwdEntry::SetTitle(const std::string& _title) {
    m_title = _title;
};


CPwdEntry::CPwdEntry() {
    m_uuid= boost::uuids::random_generator()();
}

CPwdEntry::CPwdEntry(boost::uuids::uuid _uuid) {
    m_uuid = _uuid;
}
