#pragma once

#include "MyException.h"

class RegexFilterSizeException : public MyException
{
    public:
        virtual const char* what() override;
};