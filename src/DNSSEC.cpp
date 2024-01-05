#include "DNSSEC.h"

#include <iostream>
#include <iomanip>

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    _request.addOPT();
    auto response = Communicator::domainIPFetcher(_request.getMessageInBytes());

    for(auto& b : response)
    {
        std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)b;
    }

    std::cout << std::endl;
}

// Getters

bool DNSSEC::getFilterResult() const
{
    return _filterResult;
}

std::string DNSSEC::getDestinationIP() const
{
    return destinationIP;
}
