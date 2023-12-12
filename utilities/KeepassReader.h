//
// Created by tongl on 12/12/2023.
//

#ifndef KEYBOX_KEEPASSREADER_H
#define KEYBOX_KEEPASSREADER_H

#include <vector>
#include <string>


class CKdbxReader{
public:
    bool TransformKey(const std::string& key, const std::vector<char>& seed, std::vector<char>& iv, int around);

};


#endif //KEYBOX_KEEPASSREADER_H
