#pragma once

#include "DnsMessage.hpp"
#include "Communicator.h"


class DNSSEC
{
private:
    // Fields

    DnsMessage _request;
    bool _filterResult;

public:
    // C'tor
    DNSSEC(const DnsMessage& request);

    // Getters

    bool getFilterResult() const;

private:
    // Methods

    bool validateServer(DnsMessage DNSKEY_response, DnsMessage data_response);
};
