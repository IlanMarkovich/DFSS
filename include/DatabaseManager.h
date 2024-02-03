#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <string>
#include <optional>

#include "DnsMessage.hpp"
#include "Filter.h"

#define URI "./db"
#define FILTER_DB "Filter-DB"
#define LOG_DB "Log-DB"
#define MAX_CACHE_SIZE 100000

using std::string;
using bsoncxx::builder::stream::document;

// Circular Dependency
class Filter;


class DatabaseManager
{
private:
    // Fields
    mongocxx::instance _inst;
    mongocxx::client _connection;
    int _blacklistBlocks;
    int _whitelistBlocks;
    int _cacheBlocks;

public:
    // C'tor
    DatabaseManager();

    // D'tor
    ~DatabaseManager();

    // Methods

    /// @brief Checks if a `url` is blacklisted by the user
    /// @param url The URL being checked
    /// @param isFiltering Is the context of calling this method to filter the url? 
    /// @return Is `url` blacklisted
    bool isUrlBlacklisted(const string& url, bool isFiltering);

    /// @brief Checks if a `url` is whitelisted by the user
    /// @param url The URL being checked
    /// @param isFiltering Is the context of calling this method to filter the url?
    /// @return Is `url` whitelisted
    bool isUrlWhitelisted(const string& url, bool isFiltering);

    /// @brief Insert a URL to the blacklist collection
    /// @param url The URL being inserted
    void blacklistUrl(const string& url);

    /// @brief Insert a URL to the whitelist collection
    /// @param url The URL being inserted
    void whitelistUrl(const string& url);

    /// @brief After a DNS query have been filtered, store it in the cache collection
    /// @param dnsQuery The DNS query that was filtered
    /// @param filterResult `dnsQuery`'s filter result
    void cacheDnsQuery(const struct Query& dnsQuery, bool filterResult);

    /// @brief Search for a certain query in the cache and returns its previous filter result
    /// @param dnsQuery The DNS query being searched for
    /// @return An optional variable (if there is a value it's in the cache)
    std::optional<bool> cacheQueryFilterResult(const struct Query& dnsQuery);

private:
    /// @brief Queries a DB to see if a certain `url` exists in the collection `collection`
    /// @param url The URL being queried
    /// @param collection The collection in the DB which is being searched
    /// @return Is `url` in the `collection` in the `Filter-DB` database?
    bool queryUrl(const string& url, const string& collection) const;

    /// @brief Build a `bsoncxx::builder::stream::document` from a DNS query
    /// @param dnsQuery The DNS query which the function builds a document for
    /// @return The document made from `dnsQuery`
    document buildDnsQueryDocument(const struct Query& dnsQuery) const;

    /// @brief List a certain URL in either the blacklist collection or the whitelist collection
    /// @param url The URL being insterted to either of the collections
    /// @param collection The collection `url` being inserted to
    void listUrl(const string& url, const string& collection);

    /// @brief Taks the fields of this class, information from the cache collection and more
    /// to log this session of the firewall (activates automatically in d'tor)
    /// By saving using the cache it will save the last `MAX_CACHE_SIZE` requests
    void log();
};
