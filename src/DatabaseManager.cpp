#include "DatabaseManager.h"

#include <iostream>
#include <cstdlib>
#include <thread>

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
    string dbServerCommand = "mongod --dbpath " + string(ITE_URI);
    std::thread dbServerThread(system, dbServerCommand.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits a second for the server to fully initialize before detaching the thread
    dbServerThread.detach();

    _internal_client = mongocxx::client{ mongocxx::uri{}, client_options };
}

// D'tor
DatabaseManager::~DatabaseManager()
{
    // Shuts down the iternal database server
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

bool DatabaseManager::searchUrlExternal(const string& url) const
{
    return queryUrl(url, _external_client, "External-URLs");
}

bool DatabaseManager::isUrlBlacklisted(const string & url) const
{
    return queryUrl(url, _internal_client, "Blacklist_URLs");
}

bool DatabaseManager::isUrlWhitelisted(const string & url) const
{
    return queryUrl(url, _internal_client, "Whitelist_URLs");
}

void DatabaseManager::cacheDnsQuery(const struct Query& dnsQuery, bool isValid) const
{
    //Construct a document from `dnsMsg` for the `cache` collection
    document doc = buildDnsQueryDocument(dnsQuery);
    doc << "valid" << isValid;

    _internal_client[DB]["Cache"].insert_one(doc.view());
}

std::optional<bool> DatabaseManager::cacheQueryValidation(const struct Query & dnsQuery) const
{
    document filter = buildDnsQueryDocument(dnsQuery);
    
    // If found the according query in the database return if it was filtered as valid
    if(auto document = _internal_client[DB]["Cache"].find_one(filter.view()))
    {
        return std::optional<bool>((bool)(document.value()["valid"]));
    }

    // If no query was matched from the cache return an empty optional variable
    return std::optional<bool>();
}

// Private Methods

bool DatabaseManager::queryUrl(const string& url, const mongocxx::client& dbClient, const string& collection) const
{
    document filter;
    filter << "url" << url;

    return dbClient[DB][collection].find_one(filter.view()) ? true : false;
}

document DatabaseManager::buildDnsQueryDocument(const Query & dnsQuery) const
{
    document doc;
    doc << "name" << dnsQuery.name;
    doc << "type" << dnsQuery.type;
    doc << "class" << string(reinterpret_cast<const char*>(dnsQuery.queryClass.data()));

    return doc;
}
