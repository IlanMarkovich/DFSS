#include "DNSSEC.h"

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    _request.addOPT();
    auto response = DnsMessage(Communicator::domainIPFetcher(_request.getMessageInBytes()));
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
