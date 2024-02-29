#include "DatabaseManager.h"

#include <iostream>
#include <cstdlib>
#include <thread>
#include <ctime>

// Ignore warning at the lines where initalizing the Cache collection in the c'tor
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// C'tor
DatabaseManager::DatabaseManager()
 : _blacklistBlocks(0), _whitelistBlocks(0), _cacheBlocks(0)
{
    // Set the version of the Stable API on the client.
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);

    // Host the MongoDB server using 'mongod' command in another thread
    string dbServerCommand = "mongod --dbpath " + string(URI);
    std::thread dbServerThread(system, dbServerCommand.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits a second for the server to fully initialize before detaching the thread
    dbServerThread.detach();

    _connection = mongocxx::client{ mongocxx::uri{}, client_options };
    
    // Create the cache collection at the start
    mongocxx::options::create_collection cache_options;
    cache_options.capped(true);
    cache_options.size(MAX_CACHE_SIZE);
    _connection[FILTER_DB].create_collection("Cache", cache_options);
}

// D'tor
DatabaseManager::~DatabaseManager()
{
    // Logs this session
    log();

    // Drops the cache collection
    _connection[FILTER_DB]["Cache"].drop();

    // Shuts down the internal database server
    system("(echo 'use admin' ; echo 'db.shutdownServer()') | mongosh");
}

// Public Methods

bool DatabaseManager::isUrlBlacklisted(const string & url, bool isFiltering)
{
    bool isBlacklisted = queryUrl(url, "Blacklist-URLs");
    _blacklistBlocks += (isBlacklisted && isFiltering);

    return isBlacklisted;
}

bool DatabaseManager::isUrlWhitelisted(const string & url, bool isFiltering)
{
    bool isWhitelisted = (_whitelistBlocks += queryUrl(url, "Whitelist-URLs"));
    _whitelistBlocks += (isWhitelisted && isFiltering);

    return isWhitelisted;
}

void DatabaseManager::blacklistUrl(const string & url)
{
    if(isUrlWhitelisted(url, false))
        return;

    listUrl(url, "Blacklist-URLs");
}

void DatabaseManager::whitelistUrl(const string & url)
{
    if(isUrlBlacklisted(url, false))
        return;

    listUrl(url, "Whitelist-URLs");
}

void DatabaseManager::cacheDnsQuery(const struct Query& dnsQuery, bool filterResult)
{
    //Construct a document from `dnsMsg` for the `cache` collection
    document doc = buildDnsQueryDocument(dnsQuery);
    doc << "filter_result" << filterResult;

    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    doc << "time" << std::ctime(&currentTime);

    _connection[FILTER_DB]["Cache"].insert_one(doc.view());
}

std::optional<bool> DatabaseManager::cacheQueryFilterResult(const struct Query & dnsQuery)
{
    document filter = buildDnsQueryDocument(dnsQuery);
    struct core::v1::optional<bsoncxx::v_noabi::document::value> doc;
    
    // If found the according query in the database return if it was filtered as valid
    if(doc = _connection[FILTER_DB]["Cache"].find_one(filter.view()))
    {
        _cacheBlocks++;
        return std::optional<bool>(doc->operator[]("filter_result").get_bool().value);
    }

    // If no query was matched from the cache return an empty optional variable
    return std::optional<bool>();
}

bool DatabaseManager::isFeatureTurnedOn(const std::string & feature)
{
    document filter;
    filter << "feature" << feature;
    struct core::v1::optional<bsoncxx::v_noabi::document::value> doc;

    if(doc = _connection[FILTER_DB]["Features"].find_one(filter.view()))
    {
        // Returns the status of the security feature
        return doc->operator[]("status").get_bool().value;
    }

    // If features doesn't exist, create it and make it turned on by default
    document insert;
    insert << "feature" << feature;
    insert << "status" << true;
    _connection[FILTER_DB]["Features"].insert_one(insert.view());

    return true;
}

void DatabaseManager::changeFeatureStatus(const std::string & feature)
{
    document filter;
    filter << "feature" << feature;
    struct core::v1::optional<bsoncxx::v_noabi::document::value> doc;

    if(doc = _connection[FILTER_DB]["Features"].find_one(filter.view()))
    {
        bool currentStatus = !(doc->operator[]("status").get_bool().value);

        document update;
        update << "$set" << bsoncxx::builder::stream::open_document
            << "status" << currentStatus << bsoncxx::builder::stream::close_document;
        _connection[FILTER_DB]["Features"].update_one(filter.view(), update.view());

        return;
    }

    throw std::exception();
}

std::vector<std::string> DatabaseManager::getDataList(const std::string& db, const std::string& collection) const
{
    document find;
    auto list = _connection[db][collection].find(find.view());
    std::vector<std::string> output;

    if(collection == "Blacklist-URLs" || collection == "Whitelist-URLs")
    {
        for(auto url : list)
        {
            output.push_back(std::string((url["url"].get_string().value)));
        }
    }
    else if(collection == "Cache")
    {
        for(auto cache : list)
        {
            std::string cacheStr = "Url: " + std::string(cache["name"].get_string().value) + '\n';
            cacheStr += "Filter Result: " + std::string(cache["filter_result"].get_bool().value ? "filtered" : "not filtered");
            output.push_back(cacheStr);
        }
    }
    else if(collection == "Logs")
    {
        for(auto log : list)
        {
            std::string logStr = "Blocks:\n";
            logStr += "Whitelist: " + std::string(log["whitelist_blocks"].get_string().value) + '\n';
            logStr += "Blacklist: " + std::string(log["blacklist_blocks"].get_string().value) + '\n';
            logStr += "Cache: " + std::string(log["cache_blocks"].get_string().value) + '\n';
            logStr += "External: " + std::string(log["external_blocks"].get_string().value) + '\n';
            logStr += "Phishing: " + std::string(log["phishing_blocks"].get_string().value) + '\n';
            logStr += "DNSSEC: " + std::string(log["DNSSEC_blocks"].get_string().value) + '\n';
            logStr += "SOP: " + std::string(log["SOP_blocks"].get_string().value) + '\n';
            logStr += "Amount of requests: " + std::string(log["amount_of_requests"].get_string().value) + '\n';

            logStr += "\nQueries:\n";
            auto queriesArr = log["queries_logs"].get_array().value;

            for(auto query : queriesArr)
            {
                logStr += "Name: " + std::string(query["name"].get_string().value) + '\n';
                logStr += "Filter Result: " + std::string(query["filter_result"].get_bool().value ? "filtered" : "not filtered") + '\n';
                logStr += "Data & Time: " + std::string(query["time"].get_string().value) + '\n';
            }

            output.push_back(logStr);
        }
    }

    return output;
}

// Private Methods

bool DatabaseManager::queryUrl(const string& url, const string& collection) const
{
    document filter;
    filter << "url" << url;

    return _connection[FILTER_DB][collection].find_one(filter.view()) ? true : false;
}

document DatabaseManager::buildDnsQueryDocument(const Query & dnsQuery) const
{
    document doc;
    doc << "name" << dnsQuery.name;
    doc << "type" << dnsQuery.type;
    doc << "class" << string(reinterpret_cast<const char*>(dnsQuery.queryClass));

    return doc;
}

void DatabaseManager::listUrl(const string & url, const string & collection)
{
    document doc;
    doc << "url" << url;

    // If the URL is already in the list, remove the URL from list
    if(isUrlBlacklisted(url, false) || isUrlWhitelisted(url, false))
    {
        _connection[FILTER_DB][collection].delete_one(doc.view());
        return;
    }

    _connection[FILTER_DB][collection].insert_one(doc.view());
}

void DatabaseManager::log()
{
    document doc;
    doc << "blacklist_blocks" << _blacklistBlocks;
    doc << "whitelist_blocks" << _whitelistBlocks;
    doc << "cache_blocks" << _cacheBlocks;
    doc << "external_blocks" << Filter::externalBlocks;
    doc << "phishing_blocks" << Filter::phishingBlocks;
    doc << "DNSSEC_blocks" << Filter::DNSSEC_Blocks;
    doc << "SOP_blocks" << Filter::SOP_Blocks;
    doc << "amount_of_requests" << Filter::requestAmount;

    bsoncxx::builder::basic::array arrBuilder;

    for(auto&& doc : _connection[FILTER_DB]["Cache"].find({}))
    {
        arrBuilder.append(doc);
    }

    doc << "queries_logs" << arrBuilder;
    _connection[LOG_DB]["Logs"].insert_one(doc.view());

    // Reset the variables and the cache collection
    _blacklistBlocks = 0;
    _whitelistBlocks = 0;
    _cacheBlocks = 0;
    Filter::externalBlocks = 0;
    Filter::phishingBlocks = 0;
    Filter::DNSSEC_Blocks = 0;
    Filter::SOP_Blocks = 0;
    Filter::requestAmount = 0;
    _connection[FILTER_DB]["Cache"].delete_many(bsoncxx::v_noabi::document::view_or_value());
}
