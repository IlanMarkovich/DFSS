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
    static int externalBlocks;
    static int phishingBlocks;
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
    // Methods

    bool databaseFilter() const;
    bool externalUrlFilter() const;
    bool phishingFilter() const;
};
