#pragma once

#include "DNS_Answer.h"


class DNS_DS_Answer : public DNS_Answer
{
private:
    // Fields

    std::vector<unsigned char> _key_id;
    int _algorithm;
    int _digest_type;
    std::vector<unsigned char> _digest;

public:
    // C'tor
    DNS_DS_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);
    
    // Getters

    std::vector<unsigned char> getDigest() const;

    // Methods

    virtual std::vector<unsigned char> getData() const override;
};
