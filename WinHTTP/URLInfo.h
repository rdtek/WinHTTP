#pragma once
#include <string>

using namespace std;

class URLInfo
{
public:
    string protocol;
    string host;
    string path;

    URLInfo();
    ~URLInfo();
};

