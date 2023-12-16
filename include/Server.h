#pragma once

#include "Communicator.h"

#include <mutex>

class Server
{
public:
    static std::mutex dbMutex;
private:
    Communicator m_communicator;

public:
    Server();
    void run();

private:
    void handleCommand(const string& cmd);
};
