#include "FileLoader.h"

//C'tor
FileLoader::FileLoader()
{
    string downloadCommand = "wget -O ext_url.txt " + string(BLACKBOOK_FILE);
    system(downloadCommand.c_str());

    downloadCommand = "wget -O top_100000_urls.txt " + string(TOP_URLS_FILE);
    system(downloadCommand.c_str());
}

// D'tor
FileLoader::~FileLoader()
{
    system("rm ./ext_url.txt");
    system("rm ./top_100000_urls.txt");
}
