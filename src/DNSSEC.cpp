#include "DNSSEC.h"

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    _request.addOPT();
    DnsMessage domain_A_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    if(domain_A_response.is_DNSSEC_response())
    {
        _request.changeMessageQueryType(DNS_DNSKEY);
        DnsMessage domain_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    }

    _request.changeToTLD();
    _request.changeMessageQueryType(DNS_DNSKEY);
    DnsMessage TLD_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    _request.changeMessageQueryType(DNS_DS);
    DnsMessage TLD_DS_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    _request.changeToRoot();
    _request.changeMessageQueryType(DNS_DNSKEY);
    DnsMessage ROOT_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    _request.changeMessageQueryType(DNS_DS);
    DnsMessage ROOT_DS_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
}

// Getters

bool DNSSEC::getFilterResult() const
{
    return _filterResult;
}

// Private Methods

bool DNSSEC::validateServer(DnsMessage DNSKEY_response, DnsMessage data_response)
{
    return true;
}
