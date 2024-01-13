#pragma once

#include "DnsMessage.hpp"
#include "Communicator.h"


enum DNSSEC_Level
{
    Non, Domain, TLD
};

class DNSSEC
{
private:
    // Fields

    DnsMessage _request;
    DNSSEC_Level _filterResult;

public:
    // C'tor
    DNSSEC(const DnsMessage& request);

    // Getters

    DNSSEC_Level getFilterResult() const;
};
