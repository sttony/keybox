//
// Created by tongl on 2/13/2024.
//

#include "CPwdEntry.h"


boost::property_tree::ptree CPwdEntry::toJsonObj() {
    boost::property_tree::ptree root;

    root.put("user_name", m_username);
    root.put("url", m_url);
    root.add_child("note", m_note.toJsonObj());
    root.add_child("password", m_password.toJsonObj());
    root.add_child("attachment", m_attachment.toJsonObj());

    return root;
}
