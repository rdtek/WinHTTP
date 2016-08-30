#include "StringTools.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "urlinfo.h"
using namespace std;

namespace StringTools {

    void split(const string &s, char delim, vector<string> &elems) {
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }

    vector<string> split(const string &s, char delim) {
        vector<string> elems;
        split(s, delim, elems);
        return elems;
    }
    
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

    void parseURL(string& strInputUrl, URLInfo& outUrl)
    {
        string str_url = strInputUrl;
        outUrl.protocol = "";
        outUrl.host = "";
        outUrl.path = "";
        size_t result = -1;
        int path_idx = -1;

        if (str_url.length() <= 0) return;
        trim(str_url);
        
        //Get the PROTOCOL name
        result = str_url.find("http", 0);
        if (result >= 0) {
            outUrl.protocol = "http";
            ltrim(str_url, "http");
        }

        result = str_url.find("https", 0);
        if (result >= 0) {
            outUrl.protocol = "https";
            ltrim(str_url, "https");
        }

        //Get the HOST name
        ltrim(str_url, "://");
        path_idx = str_url.find_first_of("/", 0);
        if (path_idx <= -1) path_idx = str_url.size();
        outUrl.host = str_url.substr(0, path_idx);
        
        //Get the path part
        ltrim(str_url, "www.");
        path_idx = str_url.find_first_of("/", 0);
        outUrl.path = "/";
        if (path_idx < str_url.length()) {
            int path_length = str_url.length() - path_idx;
            outUrl.path = str_url.substr(path_idx, path_length);
        }
    }
}