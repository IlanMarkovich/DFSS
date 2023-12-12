#pragma once

#include <iostream>

class ServerException : public std::exception
{
    public:
        virtual const char* what() = 0;
};
