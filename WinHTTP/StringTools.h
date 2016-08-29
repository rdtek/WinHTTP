#pragma once
#include <string>
#include <vector>
#include "urlinfo.h"

using namespace std;

namespace StringTools {

    void split(const string& s, char delim, vector<string>& elems);
    vector<string> split(const string& s, char delim);
    static char* ws = " \t\n\r\f\v";
    string& rtrim(string& s, const char* t = ws);
    string& ltrim(string& s, const char* t = ws);
    string& trim(string& s, const char* t = ws);
    void parseURL(string& strInputUrl, URLInfo& outUrl);
}


