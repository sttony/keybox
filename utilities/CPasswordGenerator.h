//
// Created by tongl on 3/24/2024.
//

#ifndef KEYBOX_CPASSWORDGENERATOR_H
#define KEYBOX_CPASSWORDGENERATOR_H
#include "InitGlobalRG.h"
#include <string>

class CPasswordGenerator {
private:
    bool m_needLower = true;
    bool m_needUpper{};
    bool m_needDigit = true;
    bool m_needMinus{}; // -
    bool m_needAdd{}; //+
    bool m_needShift1_8{}; // !@#$%^&*
    bool m_needBrace{}; // (){}[]
    bool m_needSpace{};
    bool m_needQuestion{};
    bool m_needSlash{};
    bool m_needGreaterLess{}; // <?
    RandomGenerator& m_RG;
    std::vector<char> m_pool;
    int m_length = 8;

public:
    explicit CPasswordGenerator(RandomGenerator& _rg): m_RG(_rg){

    }

    void SetLower(bool isEnabled);
    void SetUpper(bool isEnabled);
    void SetDigit(bool isEnabled);
    void SetMinus(bool isEnabled);
    void SetAdd(bool isEnabled);
    void SetShift1_8(bool isEnabled);
    void SetBrace(bool isEnabled);
    void SetSpace(bool isEnabled);
    void SetQuestion(bool isEnabled);
    void SetSlash(bool isEnabled);
    void SetGreaterLess(bool isEnabled);
    void SetLength(int);

    std::string GeneratePassword();

private:
    void resetPool();
};


#endif //KEYBOX_CPASSWORDGENERATOR_H
