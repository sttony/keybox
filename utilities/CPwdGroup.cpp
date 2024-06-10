
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>
#include "CPwdGroup.h"

boost::uuids::uuid g_RootGroupId = boost::uuids::string_generator()("60dd3126-6e7a-4e50-961b-fa77a91d7e38");

CPwdGroup g_RootGroup("Root", g_RootGroupId);

std::string CPwdGroup::GetID() const{
    return boost::uuids::to_string(m_uuid);
}

std::string CPwdGroup::GetName() const{
    return m_name;
}

CPwdGroup::CPwdGroup(std::string &name):m_name(name) {
    m_uuid = boost::uuids::random_generator()();
}

CPwdGroup::CPwdGroup(const std::string &name, boost::uuids::uuid uuid):m_name(name), m_uuid(uuid) {

}

CPwdGroup::CPwdGroup(std::string &&name): m_name(name) {
    m_uuid = boost::uuids::random_generator()();
}

CPwdGroup::CPwdGroup(std::string &&name, boost::uuids::uuid uuid): m_name(name), m_uuid(uuid) {

}

void CPwdGroup::SetName(std::string _name) {
    m_name = _name;
}

boost::property_tree::ptree CPwdGroup::toJsonObj() {
    boost::property_tree::ptree root;
    root.put("uuid", boost::uuids::to_string(m_uuid));
    root.put("name", m_name);
    return root;
}

uint32_t CPwdGroup::fromJsonObj(const boost::property_tree::ptree& jsonObj) {
    auto uuidStr = jsonObj.get<std::string>("uuid");
    m_uuid = boost::lexical_cast<boost::uuids::uuid>(uuidStr);
    m_name = jsonObj.get<std::string>("name");
    return 0;
}

boost::uuids::uuid CPwdGroup::GetUUID() const {
    return m_uuid;
}

