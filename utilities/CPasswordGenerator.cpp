//
// Created by tongl on 3/24/2024.
//

#include "CPasswordGenerator.h"
#include <string>
using namespace std;
void CPasswordGenerator::SetAdd(bool isEnabled) {
    m_needAdd = isEnabled;
}

void CPasswordGenerator::SetUpper(bool isEnabled) {
    m_needUpper = isEnabled;
}

void CPasswordGenerator::SetLower(bool isEnabled) {
    m_needLower = isEnabled;
}

void CPasswordGenerator::SetDigit(bool isEnabled) {
    m_needDigit = isEnabled;
}

void CPasswordGenerator::SetShift1_8(bool isEnabled) {
    m_needShift1_8 = isEnabled;
}

void CPasswordGenerator::SetGreaterLess(bool isEnabled) {
    m_needGreaterLess = isEnabled;
}

void CPasswordGenerator::SetMinus(bool isEnabled) {
    m_needMinus = isEnabled;
}

void CPasswordGenerator::SetBrace(bool isEnabled) {
    m_needBrace = isEnabled;
}

void CPasswordGenerator::SetSpace(bool isEnabled) {
    m_needSpace = isEnabled;
}

void CPasswordGenerator::SetQuestion(bool isEnabled) {
    m_needQuestion = isEnabled;
}

void CPasswordGenerator::SetSlash(bool isEnabled) {
    m_needSlash = isEnabled;
}

void CPasswordGenerator::resetPool() {
    m_pool.clear();
    if(m_needSlash){
        m_pool.emplace_back('/');
        m_pool.emplace_back('\\');
    }
    if(m_needQuestion){
        m_pool.emplace_back('?');
    }
    if(m_needSpace){
        m_pool.emplace_back(' ');
    }
    if(m_needBrace){
        static string braces= "[](){}";
        m_pool.insert(m_pool.end(), braces.begin(), braces.end());
    }
    if(m_needMinus){
        m_pool.emplace_back('-');
    }
    if(m_needGreaterLess){
        m_pool.emplace_back('<');
        m_pool.emplace_back('>');
    }
    if(m_needShift1_8){
        static string specials= "!@#$%^&*";
        m_pool.insert(m_pool.end(), specials.begin(), specials.end());
    }
    if(m_needDigit){
        static string digits= "0123456789";
        m_pool.insert(m_pool.end(), digits.begin(), digits.end());
    }
    if(m_needLower){
        static string lowers= "abcdefghijklmnopqrstuvwxyz";
        m_pool.insert(m_pool.end(), lowers.begin(), lowers.end());
    }
    if(m_needUpper){
        static string uppers= "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        m_pool.insert(m_pool.end(), uppers.begin(), uppers.end());
    }
    if(m_needAdd){
        m_pool.emplace_back('+');
    }


}

void CPasswordGenerator::SetLength(int _len) {
    m_length = _len;
}

string CPasswordGenerator::GeneratePassword() {
    vector<unsigned char> buff ;
    m_RG.GetNextBytes(m_length, buff);
    string result;
    for(int i =0;i <m_length; ++i){
        result += m_pool[buff[i]%m_pool.size()];
    }
    return result;
}
