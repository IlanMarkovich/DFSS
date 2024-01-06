#pragma once

#include "DnsMessage.hpp"
#include "Communicator.h"


class DNSSEC
{
private:
    // Fields

    DnsMessage _request;
    std::vector<unsigned char> _destinationIP;
    bool _filterResult;

public:
    // C'tor
    DNSSEC(const DnsMessage& request);

    // Getters

    bool getFilterResult() const;
    std::vector<unsigned char> getDestinationIP() const;

private:
    // Methods
};
