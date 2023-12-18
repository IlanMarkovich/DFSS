#include "DatabaseManager.h"

#include <iostream>
#include <cstdlib>
#include <thread>
#include <ctime>

// Ignore warning at the lines where initalizing the Cache collection in the c'tor
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// C'tor
DatabaseManager::DatabaseManager()
{
    const auto uri = mongocxx::uri{EXT_URI};

    // Set the version of the Stable API on the client.
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);
    
    // Setup the connection to the external DB
    _external_client = mongocxx::client{ uri, client_options };

    // Host the MongoDB server using 'mongod' command in another thread
    string dbServerCommand = "mongod --dbpath " + string(INT_URI);
    std::thread dbServerThread(system, dbServerCommand.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits a second for the server to fully initialize before detaching the thread
    dbServerThread.detach();

    _internal_client = mongocxx::client{ mongocxx::uri{}, client_options };
    
    // Create the cache collection at the star
    mongocxx::options::create_collection cache_options;
    cache_options.capped(true);
    cache_options.size(MAX_CACHE_SIZE);
    _internal_client[FILTER_DB].create_collection("Cache", cache_options);
}

// D'tor
DatabaseManager::~DatabaseManager()
{
    // Logs this session
    log();

    // Deletes the content of the cache collection
    _internal_client[FILTER_DB]["Cache"].drop();

    // Shuts down the internal database server
    system("(echo 'use admin' ; echo 'db.shutdownServer()') | mongosh");
}

// Public Methods

void DatabaseManager::pingExternalDatabase() const
{
    try
    {
        auto db = _external_client["admin"];

        const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
        db.run_command(ping_cmd.view());
        std::cout << "Pinged your deployment. You successfully connected to the database!" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

bool DatabaseManager::searchUrlExternal(const string& url)
{
    return queryUrl(url, _external_client, "External-URLs");
}

bool DatabaseManager::isUrlBlacklisted(const string & url, bool isFiltering)
{
    bool isBlacklisted = queryUrl(url, _internal_client, "Blacklist-URLs");
    _blacklistBlocks += (isBlacklisted && isFiltering);

    return isBlacklisted;
}

bool DatabaseManager::isUrlWhitelisted(const string & url, bool isFiltering)
{
    bool isWhitelisted = (_whitelistBlocks += queryUrl(url, _internal_client, "Whitelist-URLs"));
    _whitelistBlocks = (isWhitelisted && isFiltering);

    return isWhitelisted;
}

void DatabaseManager::blacklistUrl(const string & url)
{
    listUrl(url, "Blacklist-URLs");
}

void DatabaseManager::whitelistUrl(const string & url)
{
    listUrl(url, "Whitelist-URLs");
}

void DatabaseManager::cacheDnsQuery(const struct Query& dnsQuery, bool filterResult)
{
    //Construct a document from `dnsMsg` for the `cache` collection
    document doc = buildDnsQueryDocument(dnsQuery);
    doc << "filter_result" << filterResult;

    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    doc << "time" << std::ctime(&currentTime);

    _internal_client[FILTER_DB]["Cache"].insert_one(doc.view());
}

std::optional<bool> DatabaseManager::cacheQueryFilterResult(const struct Query & dnsQuery)
{
    document filter = buildDnsQueryDocument(dnsQuery);
    struct core::v1::optional<bsoncxx::v_noabi::document::value> doc;
    
    // If found the according query in the database return if it was filtered as valid
    if(doc = _internal_client[FILTER_DB]["Cache"].find_one(filter.view()))
    {
        _cacheBlocks++;
        return std::optional<bool>(doc->operator[]("filter_result").get_bool().value);
    }

    // If no query was matched from the cache return an empty optional variable
    return std::optional<bool>();
}

// Private Methods

bool DatabaseManager::queryUrl(const string& url, const mongocxx::client& dbClient, const string& collection) const
{
    document filter;
    filter << "url" << url;

    return dbClient[FILTER_DB][collection].find_one(filter.view()) ? true : false;
}

document DatabaseManager::buildDnsQueryDocument(const Query & dnsQuery) const
{
    document doc;
    doc << "name" << dnsQuery.name;
    doc << "type" << dnsQuery.type;
    doc << "class" << string(reinterpret_cast<const char*>(dnsQuery.queryClass.data()));

    return doc;
}

void DatabaseManager::listUrl(const string & url, const string & collection)
{
    if(isUrlBlacklisted(url, false) || isUrlWhitelisted(url, false))
    {
        std::cerr << "URL already listed!" << std::endl;
        return;
    }

    document doc;
    doc << "url" << url;

    _internal_client[FILTER_DB][collection].insert_one(doc.view());
}

void DatabaseManager::log() const
{
    document doc;
    doc << "blacklist_blocks" << _blacklistBlocks;
    doc << "whitelist_blocks" << _whitelistBlocks;
    doc << "cache_blocks" << _cacheBlocks;
    doc << "external_blocks" << _externalBlocks;
    doc << "amount_of_requests" << Filter::requestAmount;

    bsoncxx::builder::basic::array arrBuilder;

    for(auto&& doc : _internal_client[FILTER_DB]["Cache"].find({}))
    {
        arrBuilder.append(doc);
    }

    doc << "queries_logs" << arrBuilder;
    _internal_client[LOG_DB]["Logs"].insert_one(doc.view());
}
