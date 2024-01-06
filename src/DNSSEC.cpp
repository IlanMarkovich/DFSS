#include "DNSSEC.h"

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    _request.addOPT();
    DnsMessage domain_A_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    auto A_RRset = domain_A_response.getResponse_RRset<DNS_A_Answer>();

    if(A_RRset.empty())
        return; // TODO throw

    _destinationIP = A_RRset[0].get_IP_address();

    if(domain_A_response.is_DNSSEC_response())
    {
        _request.changeMessageQueryType(DNS_DNSKEY);
        DnsMessage domain_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    }
}

// Getters

bool DNSSEC::getFilterResult() const
{
    return _filterResult;
}

std::vector<unsigned char> DNSSEC::getDestinationIP() const
{
    return _destinationIP;
}
