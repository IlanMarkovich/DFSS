#pragma once

#include "DNS_Answer.h"


class DNS_DNSKEY_Answer : public DNS_Answer
{
private:
    // Fields

    std::vector<unsigned char> _flags;
    int _protocol;
    int _algorithm;
    std::vector<unsigned char> _public_key;

public:
    // C'tor
    DNS_DNSKEY_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);
};
