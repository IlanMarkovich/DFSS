#include "MyException.h"

class socketCreationException : public MyException
{
    public:
        virtual const char* what() override;
};