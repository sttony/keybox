

#include "CSettings.h"
#include "utilities/error_code.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace fs = boost::filesystem;
using namespace std;

#ifdef _WIN32
#define  HOME "HOMEPATH"
#else
#define HOME "HOME"
#endif

uint32_t CSettings::Load() {
    fs::path dir(getenv(HOME));
    fs::path file(".keybox.settings");
    fs::path full_path = dir / file;

    if (!boost::filesystem::exists(full_path)) {
        return ERROR_FILE_IO;
    }
    boost::property_tree::ptree settings_pt;
    ifstream ifs(full_path.string(), ios_base::in);
    try {
        boost::property_tree::read_json(ifs, settings_pt);
    } catch (const boost::property_tree::json_parser_error &e) {
        return ERROR_INVALID_JSON;
    }

    m_last_keybox_file_path = settings_pt.get<std::string>(LAST_KEYBOX_FILE_PATH_KEY);
}

void CSettings::Save() {
    fs::path dir(getenv(HOME));
    fs::path file(".keybox.settings");
    fs::path full_path = dir / file;

    boost::property_tree::ptree settings_pt;
    settings_pt.put(LAST_KEYBOX_FILE_PATH_KEY,m_last_keybox_file_path);
    ofstream ofs(full_path.string(), ios_base::in);
    boost::property_tree::write_json(ofs, settings_pt);
    ofs.close();
}

const std::string &CSettings::GetLastKeyboxFilepath() {
    return m_last_keybox_file_path;
}

void CSettings::SetLastKeyboxFilepath(const std::string& file) {
    m_last_keybox_file_path = file;
}
