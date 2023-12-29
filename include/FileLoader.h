#pragma once

#include <string>
#include <cstdlib>

#define BLACKBOOK_FILE "https://raw.githubusercontent.com/stamparm/blackbook/master/blackbook.txt"
#define TOP_URLS_FILE "https://raw.githubusercontent.com/zer0h/top-1000000-domains/master/top-100000-domains"

using std::string;


class FileLoader
{
public:
    // C'tor
    FileLoader();

    // D'tor
    ~FileLoader();
};
