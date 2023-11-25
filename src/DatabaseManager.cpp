#include "DatabaseManager.h"

#include <iostream>

// C'tor
DatabaseManager::DatabaseManager()
{
    const auto uri = mongocxx::uri{URI};

    // Set the version of the Stable API on the client.
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);
    
    // Setup the connection and get a handle on the "admin" database.
    _client = mongocxx::client{ uri, client_options };
    _db = _client["admin"];
}

// Public Methods

void DatabaseManager::pingDatabase()
{
    try
    {
        const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
        _db.run_command(ping_cmd.view());
        std::cout << "Pinged your deployment. You successfully connected to the database!" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
