#pragma once

#include "MyException.h"

class SocketCreationException : public MyException
{
    public:
        virtual const char* what() override;
};