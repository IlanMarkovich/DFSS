#pragma once

#include "DnsMessage.h"
#include "Communicator.h"


class DNSSEC
{
private:
    // Fields

    DnsMessage _request;
    std::string destinationIP;
    bool _filterResult;

public:
    // C'tor
    DNSSEC(const DnsMessage& request);

    // Getters

    bool getFilterResult() const;
    std::string getDestinationIP() const;
};
