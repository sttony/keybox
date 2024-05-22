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
    CRandomGenerator& m_RG;
    std::vector<std::string> m_pool;
    int m_length = 8;

public:
    explicit CPasswordGenerator(CRandomGenerator& _rg): m_RG(_rg){
        resetPool();
    }

    void SetLower(bool isEnabled);
    bool GetLower();

    void SetUpper(bool isEnabled);
    bool GetUpper();

    void SetDigit(bool isEnabled);
    bool GetDigit();

    void SetMinus(bool isEnabled);
    bool GetMinus();

    void SetAdd(bool isEnabled);
    bool GetAdd();

    void SetShift1_8(bool isEnabled);
    bool GetShift1_8();

    void SetBrace(bool isEnabled);
    bool GetBrace();


    void SetSpace(bool isEnabled);
    bool GetSpace();

    void SetQuestion(bool isEnabled);
    bool GetQuestion();

    void SetSlash(bool isEnabled);
    bool GetSlash();

    void SetGreaterLess(bool isEnabled);
    bool GetGreaterLess();

    void SetLength(int);
    int GetLength();

    std::string GeneratePassword();

private:
    void resetPool();
};


#endif //KEYBOX_CPASSWORDGENERATOR_H
