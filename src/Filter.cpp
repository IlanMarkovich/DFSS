#include "Filter.h"

#include <mutex>
#include <fstream>

#include "Server.h"
#include "DNSSEC.h"
#include "RegexFilter.h"

// Init static variables
int Filter::requestAmount = 0;
int Filter::externalBlocks = 0;

// C'tor
Filter::Filter(const DnsMessage & dnsReq, DatabaseManager & dbManagger)
 : _dnsReq(dnsReq), _dbManager(dbManagger)
{
    requestAmount++;
    _filterResult = databaseFilter();
    
    if(!_filterResult)
    {
        // Check if the DNS request's query is saved in the cache collection
        if(_dbManager.isFeatureTurnedOn("cache"))
        {
            std::optional<bool> prevFilterResult;

            if((prevFilterResult = _dbManager.cacheQueryFilterResult(dnsReq.getQuery())).has_value())
            {
                _filterResult = prevFilterResult.value();
                return;
            }
        }

        _filterResult = externalUrlFilter()
            || phishingFilter()
            || DNSSEC_Filter();
    }

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
    if(!_dbManager.isFeatureTurnedOn("blwl"))
        return false;

    string url = _dnsReq.getQuery().name;

    // Protect the `_dbManager` resource which can be accessed by other threads
    std::lock_guard<std::mutex> dbLock(Server::dbMutex);

    return !_dbManager.isUrlWhitelisted(url, true) && _dbManager.isUrlBlacklisted(url, true);
}

bool Filter::externalUrlFilter() const
{
    if(!_dbManager.isFeatureTurnedOn("external"))
        return false;

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
    if(!_dbManager.isFeatureTurnedOn("phishing"))
        return false;

    bool result = false;

    try
    {
        RegexFilter filter(_dnsReq.getQuery().name);

        std::ifstream file("./top_domains.txt");
        string url;

        while(std::getline(file, url) && !result)
        {
            if(filter.Filter(url))
            {
                externalBlocks++;
                result = true;
            }
        }

        file.close();
    }
    catch(const std::exception& e)
    {
        return false;
    }

    return result;
}

bool Filter::DNSSEC_Filter() const
{
    if(!_dbManager.isFeatureTurnedOn("DNSSEC"))
        return false;

    // Does the DNSSEC communication and authentication verification
    DNSSEC dnssec(_dnsReq);

    return dnssec.getFilterResult() == TLD && dnssec.getFilterResult() == Domain;
}
