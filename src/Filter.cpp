#include "Filter.h"

// C'tor
Filter::Filter(const DnsMessage & dnsReq, const DatabaseManager& dbManagger)
 : _dnsReq(dnsReq), _dbManager(dbManagger)
{
    _filterResult = databaseFilter();
}

// Getters

bool Filter::getFilterResult() const
{
    return _filterResult;
}

// Private Methods

bool Filter::databaseFilter() const
{
    string url = _dnsReq.getQuery().name;

    return _dbManager.searchUrlExternal(url);
}
