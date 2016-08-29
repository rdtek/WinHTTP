#pragma once
#include <string>
using namespace std;

namespace StringTools {
    static char* ws = " \t\n\r\f\v";
    string& rtrim(string& s, const char* t = ws);
    string& ltrim(string& s, const char* t = ws);
    string& trim(string& s, const char* t = ws);
    void parseURL(string& strInput);
}


