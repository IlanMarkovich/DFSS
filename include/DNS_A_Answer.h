#pragma once

#include "DNS_Answer.h"


class DNS_A_Answer : public DNS_Answer
{
private:
    // Field
    std::string _ip_address;

public:
    // C'tor
    DNS_A_Answer(const std::string& name, const std::vector<unsigned char>& dnsMsg, int& index);
};
