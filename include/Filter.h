#pragma once

#include "DnsMessage.h"
#include "DatabaseManager.h"

// Circular Dependency
class DatabaseManager;


class Filter
{
public:
    // Fields
    static int requestAmount;
private:
    DnsMessage _dnsReq;
    DatabaseManager& _dbManager;
    bool _filterResult;

public:
    // C'tor
    Filter(const DnsMessage& dnsReq, DatabaseManager& dbManager);

    // Getter
    bool getFilterResult() const;

private:
    bool databaseFilter() const;
};
