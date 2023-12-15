#pragma once

#include "DnsMessage.h"
#include "DatabaseManager.h"


class Filter
{
private:
    // Fields
    DnsMessage _dnsReq;
    const DatabaseManager& _dbManager;
    bool _filterResult;

public:
    // C'tor
    Filter(const DnsMessage& dnsReq, const DatabaseManager& dbManager);

    // Getter
    bool getFilterResult() const;

private:
    bool databaseFilter() const;
};
