//
// Created by tongl on 3/24/2024.
//

#include "CPasswordGenerator.h"
#include <string>
using namespace std;
void CPasswordGenerator::SetAdd(bool isEnabled) {
    m_needAdd = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetAdd() {
    return m_needAdd;
}

void CPasswordGenerator::SetUpper(bool isEnabled) {
    m_needUpper = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetUpper() {
    return m_needUpper;
}

void CPasswordGenerator::SetLower(bool isEnabled) {
    m_needLower = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetLower() {
    return m_needLower;
}

void CPasswordGenerator::SetDigit(bool isEnabled) {
    m_needDigit = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetDigit() {
    return m_needDigit;
}

void CPasswordGenerator::SetShift1_8(bool isEnabled) {
    m_needShift1_8 = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetShift1_8() {
    return m_needShift1_8;
}

void CPasswordGenerator::SetGreaterLess(bool isEnabled) {
    m_needGreaterLess = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetGreaterLess() {
    return m_needGreaterLess;
}

void CPasswordGenerator::SetMinus(bool isEnabled) {
    m_needMinus = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetMinus() {
    return m_needMinus;
}

void CPasswordGenerator::SetBrace(bool isEnabled) {
    m_needBrace = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetBrace() {
    return m_needBrace;
}

void CPasswordGenerator::SetSpace(bool isEnabled) {
    m_needSpace = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetSpace() {
    return m_needSpace;
}

void CPasswordGenerator::SetQuestion(bool isEnabled) {
    m_needQuestion = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetQuestion() {
    return m_needQuestion;
}

void CPasswordGenerator::SetSlash(bool isEnabled) {
    m_needSlash = isEnabled;
    resetPool();
}
bool CPasswordGenerator::GetSlash() {
    return m_needSlash;
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
int CPasswordGenerator::GetLength() {
    return m_length;
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
