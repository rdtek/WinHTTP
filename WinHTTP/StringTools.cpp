#include "StringTools.h"
#include <string>
#include <iostream>

using namespace std;

namespace StringTools {

    //Trim functions adapted from 
    //http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

    // trim from end of string (right)
    string& rtrim(string& s, const char* t)
    {
        s.erase(s.find_last_not_of(ws) + 1);
        return s;
    }

    // trim from beginning of string (left)
    string& ltrim(string& s, const char* t)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (left & right)
    string& trim(string& s, const char* t)
    {
        return ltrim(rtrim(s, t), t);
    }

    void parseURL(string& strInput)
    {
        //if (strInput.length <= 0) return;
        cout << "\n[" << strInput << "]";
        trim(strInput);
        cout << "\n[" << strInput << "]";
        size_t result = strInput.find("http", 0);

        //TODO: extract and save the protocol e.g. http/https
        //TODO: extract and save the hostname
        //TODO: extract and save path after hostname

        printf("\nRESULT = %d", result);
    }
}