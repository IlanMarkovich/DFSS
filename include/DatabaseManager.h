#pragma once

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#define URI "mongodb+srv://ADMIN:Pass123@iep-db.dc3povw.mongodb.net/?retryWrites=true&w=majority"

class DatabaseManager
{
private:
    // FIELDS
    mongocxx::instance inst;
    mongocxx::client client;
    mongocxx::database db;

public:
    // C'tor
    DatabaseManager();

    // Methods
    void pingDatabase() const;
};
