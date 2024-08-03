
#ifndef KEYBOX_CSETTINGS_H
#define KEYBOX_CSETTINGS_H

#include <string>
#include <cstdint>

class CSettings {
private:
    const std::string LAST_KEYBOX_FILE_PATH_KEY = "last_keybox_file_path";
    std::string m_last_keybox_file_path;

public:
    uint32_t Load();
    void Save();
    const std::string& GetLastKeyboxFilepath();
    void SetLastKeyboxFilepath(const std::string& file);
};


#endif //KEYBOX_CSETTINGS_H
