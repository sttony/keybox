//
// Created by tongl on 12/12/2023.
//

#ifndef KEYBOX_KEEPASSREADER_H
#define KEYBOX_KEEPASSREADER_H

#include <vector>
#include <string>


class CKdbxReader{
public:
    uint32_t TransformKey(const std::string& key,
                          const std::vector<unsigned char>& seed,
                          int uNumRounds,
                          std::vector<unsigned char>& output);

};


#endif //KEYBOX_KEEPASSREADER_H
