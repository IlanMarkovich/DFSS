#include "Filter.h"

#include <mutex>
#include <fstream>

#include "Server.h"
#include "RegexFilter.h"
#include "ServerException.h"

// Init static variables
int Filter::requestAmount = 0;
int Filter::externalBlocks = 0;
int Filter::phishingBlocks = 0;

// C'tor
Filter::Filter(const DnsMessage & dnsReq, DatabaseManager & dbManagger)
 : _dnsReq(dnsReq), _dbManager(dbManagger)
{
    requestAmount++;
    
    // Check if the DNS request's query is saved in the cache collection
    {
        std::optional<bool> prevFilterResult;

        if((prevFilterResult = _dbManager.cacheQueryFilterResult(dnsReq.getQuery())).has_value())
        {
            _filterResult = prevFilterResult.value();
            return;
        }
    }

    bool dbFilter = false, externalFilter = false, phishingFilterVar = false;

    _filterResult = (dbFilter = databaseFilter())
        || (externalFilter = externalUrlFilter())
        || (phishingFilterVar = phishingFilter());

    string filterMethod = dbFilter ? "database"
        : externalFilter ? "external"
        : phishingFilterVar ? "phishing"
        : "";

    // Cache the query and the result in the cache collection in the database
    _dbManager.cacheDnsQuery(dnsReq.getQuery(), _filterResult, filterMethod);
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

    // Protect the `_dbManager` resource which can be accessed by other threads
    std::lock_guard<std::mutex> dbLock(Server::dbMutex);

    return !_dbManager.isUrlWhitelisted(url, true) && _dbManager.isUrlBlacklisted(url, true);
}

bool Filter::externalUrlFilter() const
{
    bool result = false;

    std::ifstream file("./ext_url.txt");
    string url;

    // Search for the url in the malicious URL file
    while(std::getline(file, url) && !result)
    {
        if(_dnsReq.getQuery().name == url)
        {
            externalBlocks++;
            result = true;
        }
    }

    file.close();

    return result;
}

bool Filter::phishingFilter() const
{
    bool result = false;

    std::ifstream file("./top_100000_urls.txt");
    string url;

    RegexFilter filter(_dnsReq.getQuery().name);

    while(std::getline(file, url) && !result)
    {
        if(filter.Filter(url))
        {
            phishingBlocks++;
            result = true;
        }
    }

    file.close();
    return result;
}
