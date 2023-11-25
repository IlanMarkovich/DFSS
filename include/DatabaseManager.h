#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <string>

#define URI "mongodb+srv://ADMIN:Pass123@iep-db.dc3povw.mongodb.net/?retryWrites=true&w=majority"

using std::string;

class DatabaseManager
{
private:
    // FIELDS
    mongocxx::instance _inst;
    mongocxx::client _client;

public:
    // C'tor
    DatabaseManager();

    // Methods
    
    /// @brief Pings the database and prints when successful
    void pingDatabase() const;

    bool searchUrl(const string& url) const;
};
