#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <string>

#define EXT_URI "mongodb+srv://ADMIN:Pass123@iep-db.dc3povw.mongodb.net/?retryWrites=true&w=majority"
#define ITE_URI "./db"
#define OK 0

using std::string;

class DatabaseManager
{
private:
    // Fields
    mongocxx::instance _inst;
    mongocxx::client _external_client;
    mongocxx::client _internal_client;

public:
    // C'tor
    DatabaseManager();

    // D'tor
    ~DatabaseManager();

    // Methods
    
    /// @brief Pings the database and prints when successful
    void pingDatabase() const;

    /// @brief Searches a URL in the external database and determines if the url is there
    /// @param url The searched URL
    /// @return Is the URL in the external database
    bool searchUrlExternal(const string& url) const;

    /// @brief Checks if a `url` is blacklisted by the user
    /// @param url The URL being checked
    /// @return Is `url` blacklisted
    bool isUrlBlacklisted(const string& url) const;

    /// @brief Checks if a `url` is whitelisted by the user
    /// @param url The URL being checked
    /// @return Is `url` whitelisted
    bool isUrlWhitelisted(const string& url) const;

private:
    /// @brief Queries a DB to see if a certain `url` exists in the collection `collection`
    /// @param url The URL being queried
    /// @param dbClient The client which performs the query
    /// @param collection The collection in the DB which is being searched
    /// @return Is `url` in the `collection` in the `Filter-DB` database?
    bool queryUrl(const string& url, const mongocxx::client& dbClient, const string& collection) const;
};
