#pragma once

#include <string>
#include <cstdlib>

#define BLACKBOOK_FILE "https://raw.githubusercontent.com/stamparm/blackbook/master/blackbook.txt"
#define TOP_DOMAINS "https://raw.githubusercontent.com/bensooter/URLchecker/master/top-1000-websites.txt"

using std::string;


class FileLoader
{
public:
    // C'tor
    FileLoader();

    // D'tor
    ~FileLoader();
};
