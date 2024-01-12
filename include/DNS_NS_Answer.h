#pragma once

#include "DNS_Answer.h"


class DNS_NS_Answer : public DNS_Answer
{
private:
    // Fields

    std::string _name_server;

public:
    // C'tor
    DNS_NS_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);

    // Getters

    std::string getNameServer() const;
};
