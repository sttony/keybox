
#include "CPasswordGenerator.h"
#include <string>
#include <unordered_set>
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
        m_pool.emplace_back("/\\");
    }
    if(m_needQuestion){
        m_pool.emplace_back("?");
    }
    if(m_needSpace){
        m_pool.emplace_back(" ");
    }
    if(m_needBrace){
        m_pool.emplace_back("[](){}");
    }
    if(m_needMinus){
        m_pool.emplace_back("-");
    }
    if(m_needGreaterLess){
        m_pool.emplace_back("<>");
    }
    if(m_needShift1_8){
        m_pool.emplace_back("!@#$%^&*");
    }
    if(m_needDigit){
        m_pool.emplace_back("0123456789");
    }
    if(m_needLower){
        m_pool.emplace_back("abcdefghijklmnopqrstuvwxyz");
    }
    if(m_needUpper){
        m_pool.emplace_back("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }
    if(m_needAdd){
        m_pool.emplace_back("+");
    }


}

void CPasswordGenerator::SetLength(int _len) {
    m_length = _len;
    resetPool();
}
int CPasswordGenerator::GetLength() {
    return m_length;
}

string CPasswordGenerator::GeneratePassword() {
    if( m_pool.empty()){
        return "";
    }
    string result(m_length, -1);
    int resultIdx = 0;

    unordered_set<int> idxToFill;
    for(int i =0; i< m_length; ++i){
        idxToFill.insert(i);
    }
    // fill one group at least one
    {
        unordered_set<int> idxGroups;
        for(int i =0; i< m_pool.size(); ++i){
            idxGroups.insert(i);
        }
        for (int i = 0; i < m_pool.size(); ++i) {
            // pickup a group;
            int idxGroup = m_RG.GetNextInt32()% idxGroups.size();
            auto itGroup = idxGroups.begin();
            for(int j = 0; j< idxGroup; ++j){
                itGroup++;
            }
            const string& group = m_pool[*itGroup];

            // pick up a position
            auto itPos = idxToFill.begin();
            uint32_t pos = m_RG.GetNextInt32() % idxToFill.size();
            for(int j = 0; j< pos; ++j){
                itPos ++;
            }
            result[*itPos] = group[m_RG.GetNextInt32() % group.size()];

            idxGroups.erase(itGroup);
            idxToFill.erase(itPos);
            resultIdx++;
            if (resultIdx >= m_length)
                break;
        }
    }

    // fill remain with uniform distribution for all
    {
        int totalSumSize = 0;
        for (auto group: m_pool) {
            totalSumSize += group.size();
        }
        for (int i = resultIdx; i < m_length; ++i) {
            // pick upa position
            auto itPos = idxToFill.begin();
            uint32_t pos = m_RG.GetNextInt32() % idxToFill.size();
            for(int j = 0; j< pos; ++j){
                itPos ++;
            }

            int rg = m_RG.GetNextInt32() % totalSumSize;
            char rr = 0;
            for (auto &group: m_pool) {
                bool isBreak = false;
                for (char c: group) {
                    rr = c;
                    if (rg-- == 0) {
                        isBreak = true;
                        break;
                    }
                }
                if (isBreak) {
                    break;
                }
            }
            result[*itPos] = rr;
            idxToFill.erase(itPos);
        }
    }
    return result;
}
