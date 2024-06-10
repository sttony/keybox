

#ifndef KEYBOX_CPWDGROUP_H
#define KEYBOX_CPWDGROUP_H

#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/property_tree/ptree.hpp>

class CPwdGroup {
public:
    std::string GetID() const;
    boost::uuids::uuid GetUUID() const;
    std::string GetName() const;
    void SetName(std::string);

    CPwdGroup(std::string& name);
    CPwdGroup(std::string&& name);
    CPwdGroup(const std::string& name, boost::uuids::uuid uuid);
    CPwdGroup(std::string&& name, boost::uuids::uuid uuid);

    boost::property_tree::ptree toJsonObj();
    uint32_t fromJsonObj(const boost::property_tree::ptree &);
private:
    boost::uuids::uuid m_uuid;
    std::string m_name;

};

extern CPwdGroup g_RootGroup;
extern boost::uuids::uuid g_RootGroupId;

#endif //KEYBOX_CPWDGROUP_H
