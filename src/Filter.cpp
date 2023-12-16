#include "Filter.h"

// C'tor
Filter::Filter(const DnsMessage & dnsReq, const DatabaseManager & dbManagger)
 : _dnsReq(dnsReq), _dbManager(dbManagger)
{
    // Check if the DNS request's query is saved in the cache collection
    {
        std::optional<bool> prevFilterResult;

        if((prevFilterResult = _dbManager.cacheQueryFilterResult(dnsReq.getQuery())).has_value())
        {
            _filterResult = prevFilterResult.value();
            return;
        }
    }

    _filterResult = databaseFilter();

    // Cache the query and the result in the cache collection in the database
    _dbManager.cacheDnsQuery(dnsReq.getQuery(), _filterResult);
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

    return !_dbManager.isUrlWhitelisted(url) &&
    (_dbManager.isUrlBlacklisted(url) || _dbManager.searchUrlExternal(url));
}
