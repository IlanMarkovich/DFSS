#pragma once

#include "DNS_Answer.h"


class DNS_A_Answer : public DNS_Answer
{
private:
    // Field
    std::vector<unsigned char> _ip_address;

public:
    // C'tor
    DNS_A_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);

    // Getters
    
    std::vector<unsigned char> get_IP_address() const;

    // Methods

    virtual std::vector<unsigned char> getData() const override;
};
