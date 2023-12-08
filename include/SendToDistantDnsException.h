#pragma once

#include "MyException.h"

class SendToDistantDnsException : public MyException
{
    public:
        virtual const char* what() override;
};