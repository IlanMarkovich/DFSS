#pragma once

#include "DNS_Answer.h"


class DNS_SOA_Answer : public DNS_Answer
{
private:
    // Fields

    std::string _primary_name_server;
    std::string _authority_mailbox;
    std::vector<unsigned char> _serial_number;
    int _refresh_interval;
    int _retry_interval;
    int _expire_limit;
    int _minimum_ttl;

public:
    // C'tor
    DNS_SOA_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);

    // Getters

    std::string getNameServer() const;
};
