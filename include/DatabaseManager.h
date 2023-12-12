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
    // FIELDS
    mongocxx::instance _inst;
    mongocxx::client _external_client;
    mongocxx::client _iternal_client;

public:
    // C'tor
    DatabaseManager();

    // D'tor
    ~DatabaseManager();

    // Methods
    
    /// @brief Pings the database and prints when successful
    void pingDatabase() const;

    /// @brief Searches a URL in the database and determines if the url is there
    /// @param url The searched URL
    /// @return Is the URL in the database
    bool searchUrl(const string& url) const;
};
