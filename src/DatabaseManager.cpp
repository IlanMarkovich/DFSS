#include "DatabaseManager.h"

#include <iostream>
#include <cstdlib>
#include <thread>

#include "SocketBindException.h"

// C'tor
DatabaseManager::DatabaseManager()
{
    const auto uri = mongocxx::uri{EXT_URI};

    // Set the version of the Stable API on the client.
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);
    
    // Setup the connection.
    _external_client = mongocxx::client{ uri, client_options };

    string dbServerCommand = "mongod --dbpath " + string(ITE_URI);
    std::thread dbServerThread(system, dbServerCommand.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    dbServerThread.detach();

    _iternal_client = mongocxx::client{ mongocxx::uri{}, client_options };
}

// D'tor
DatabaseManager::~DatabaseManager()
{
    // Shuts down the iternal database server
    system("(echo 'use admin' ; echo 'db.shutdownServer()') | mongosh");
}

// Public Methods

void DatabaseManager::pingDatabase() const
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

bool DatabaseManager::searchUrl(const string& url) const
{
    auto db = _external_client["Filter-DB"];
    
    bsoncxx::builder::stream::document filter;
    filter << "url" << url;

    auto query = db["External-URLs"].find_one(filter.view());
    return query ? true : false;
}
