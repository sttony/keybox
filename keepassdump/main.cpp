#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <pugixml.hpp>
#include "utilities/KeepassReader.h"
#include "utilities/CipherEngine.h"
#include "utilities/CGZcompressor.h"
#include "utilities/RandomGenerator.h"
#include "utilities/Base64Coder.h"
#include "help.h"

using namespace std;

struct Block {
    uint32_t blockIndex;
    unsigned char sha_256_hash[32];
    uint32_t block_size;
};


struct PasswordEntry {
    string Uuid;
    string Note;
    string Title;
    string Url;
    string UserName;
    string Password;
    vector<string> history_password;
};


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "please specify your kdbx file name, like  \n"
                        "keepass your.kdbx");
    }
    FILE *fp = fopen(argv[1], "rb+");
    if (fp == nullptr) {
        fprintf(stderr, "open file %s failed, \n", strerror(errno));
        return errno;
    }

    CKdbxReader header;
    header.LoadHeader(fp);
    vector<unsigned char> derivation_seed = header.getTransformSeed();
    uint32_t derivation_round = header.getTransformRounds();
    vector<unsigned char> masterSeed = header.getMasterSeed();
    vector<unsigned char> primay_key;

    CCipherEngine cipherEngine;
    cout << "Please enter password for kdbx file" << endl;
    string password = read_password_from_console();
    uint32_t status = cipherEngine.KeepassDerivativeKey(
            password,
            derivation_seed,
            derivation_round,
            masterSeed,
            primay_key
    );
    password.clear();
    if (status) {
        fprintf(stderr, "Derivative key failed, please try again \n");
        return status;
    }

    uint32_t pay_load_offset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    vector<unsigned char> pay_load_buff(ftell(fp) - pay_load_offset);
    fseek(fp, pay_load_offset, SEEK_SET);
    fread(&pay_load_buff[0], 1, pay_load_buff.size(), fp);
    fclose(fp);

    vector<unsigned char> decrypted;
    vector<unsigned char> iv = header.getEncryptionIV();
    status = cipherEngine.AES256EnDecrypt(&pay_load_buff[0],
                                          pay_load_buff.size(),
                                          primay_key,
                                          iv,
                                          CCipherEngine::AES_CHAIN_MODE_CBC,
                                          CCipherEngine::AES_PADDING_MODE_PKCS7,
                                          false,
                                          decrypted);
    if (status) {
        fprintf(stderr, "Decrypt file \n");
        return status;
    }

    auto *block1 = (Block *) &decrypted[32];
    CGZcompressor gzCompressor;
    if (gzCompressor.reset()) {
        cout << "Reset zlib failed" << endl;
    };
    RandomGenerator randomGenerator(RandomGenerator::Salsa20);
    std::array<unsigned char, 8> vIV = {0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a};
    randomGenerator.init(header.getInnerRandomStreamKey(), vIV);
    Base64Coder base64Coder;

    vector<unsigned char> uncompressed(1024 * 1024);
    gzCompressor.decompressData(&decrypted[sizeof(Block) + 32], block1->block_size, &uncompressed[0], 1024 * 1024);
    vector<PasswordEntry> entries;

    pugi::xml_document doc;

    // Load the XML file
    pugi::xml_parse_result result = doc.load_string((char *) &uncompressed[0]);

    // Check if the file is loaded successfully
    if (!result) {
        std::cerr << "Error loading XML file: " << result.description() << std::endl;
        return 1;
    }

    const pugi::xml_node root = doc.root().child("KeePassFile").child("Root");

    const pugi::xml_node root_group = root.child("Group");
    for (const pugi::xml_node &pw_entry: root_group.children()) {
        if (string(pw_entry.name()) == "Entry") {
            entries.emplace_back();
            for (const pugi::xml_node &property: pw_entry.children()) {
                if (string(property.name()) == "UUID") {
                    entries.back().Uuid = property.child_value();
                } else if (string(property.name()) == "String") {
                    string key = property.child("Key").child_value();
                    pugi::xml_node value_node = property.child("Value");
                    if (key == "Notes") {
                        entries.back().Note = value_node.child_value();
                    } else if (key == "Title") {
                        entries.back().Title = value_node.child_value();
                    } else if (key == "URL") {
                        entries.back().Url = value_node.child_value();
                    } else if (key == "UseName") {
                        entries.back().UserName = value_node.child_value();
                    } else if (key == "Password") {
                        string raw_password = value_node.child_value();
                        if (raw_password.size() > 0) {
                            vector<unsigned char> decoded_password;
                            base64Coder.Decode(raw_password, decoded_password);
                            vector<unsigned char> one_pad;
                            randomGenerator.GetNextBytes(decoded_password.size(), one_pad);

                            for (int i = 0; i < decoded_password.size(); ++i) {
                                decoded_password[i] = decoded_password[i] ^ one_pad[i];
                            }

                            entries.back().Password = string((char *) &decoded_password[0], decoded_password.size());
                        }
                    }
                }
                else if (string(property.name()) == "History") {
                    for (const pugi::xml_node &history_pw_entry: property.children()) {
                        if (string(pw_entry.name()) == "Entry") {
                            for (const pugi::xml_node &history_property: history_pw_entry.children()) {
                                if (string(history_property.name()) == "String") {
                                    string key = history_property.child("Key").child_value();
                                    pugi::xml_node value_node = history_property.child("Value");
                                    if (key == "Password") {
                                        string raw_password = value_node.child_value();
                                        if (raw_password.size() > 0) {
                                            vector<unsigned char> decoded_password;
                                            base64Coder.Decode(raw_password, decoded_password);
                                            vector<unsigned char> one_pad;
                                            randomGenerator.GetNextBytes(decoded_password.size(), one_pad);

                                            for (int i = 0; i < decoded_password.size(); ++i) {
                                                decoded_password[i] = decoded_password[i] ^ one_pad[i];
                                            }

                                            entries.back().history_password.emplace_back((char *) &decoded_password[0], decoded_password.size());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }


        }
    }

    for (auto password_entry: entries) {
        cout << "==========================================" << endl;
        cout << password_entry.Title << endl;
        cout << "\t" << "UserName: " << password_entry.UserName << endl;
        cout << "\t" << "Password: " << password_entry.Password << endl;
        cout << "\t" << "Url: " << password_entry.Url << endl;
        cout << "\t" << "Note: " << password_entry.Note << endl;
    }
    return 0;
}