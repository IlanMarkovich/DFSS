#include "ServerException.h"


class DatabaseServerCreationException : public ServerException
{
public:
    virtual const char* what() override;
};
