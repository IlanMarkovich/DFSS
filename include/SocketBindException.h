#include "MyException.h"

class SocketBindException : public MyException
{
    public:
        virtual const char* what() override;
};