#include "DNSSEC.h"

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    _request.addOPT();
    //DnsMessage response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    _request.changeToTLD();

    _request.changeMessageQueryType(DNS_DNSKEY);
    Communicator::DNS_ResponseFetcher(_request.getMessageInBytes());

    _request.changeMessageQueryType(DNS_DS);
    Communicator::DNS_ResponseFetcher(_request.getMessageInBytes());
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
