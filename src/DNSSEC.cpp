#include "DNSSEC.h"

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    std::string queryName = _request.getQuery().name;
    
    // Get the DNSSEC response about the full domain (and DS from TLD if supports DNSSEC)
    _request.addOPT();
    _request.changeMessageQueryType(DNS_A);
    DnsMessage domain_A_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    DnsMessage* domain_DNSKEY_response = nullptr;
    DnsMessage* TLD_DS_response = nullptr;

    if(domain_A_response.is_DNSSEC_response())
    {
        _request.changeMessageQueryType(DNS_DNSKEY);
        domain_DNSKEY_response = new DnsMessage(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

        _request.changeMessageQueryType(DNS_DS);
        TLD_DS_response = new DnsMessage(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

        delete domain_DNSKEY_response;
        delete TLD_DS_response;
    }
    // ------------------------------------------------------------------------------------------

    // Get the DNSSEC response from the TLD
    _request.changeMessageQueryType(DNS_DNSKEY);
    _request.changeQueryName(DnsMessage::getTLD(queryName));
    DnsMessage TLD_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    // ------------------------------------------------------------------------------------------

    // Get the DNSSEC response from the Root
    _request.changeMessageQueryType(DNS_DS);
    DnsMessage root_DS_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    _request.changeMessageQueryType(DNS_DNSKEY);
    _request.changeQueryNameToRoot();
    DnsMessage root_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    // ------------------------------------------------------------------------------------------
}

// Getters

DNSSEC_Level DNSSEC::getFilterResult() const
{
    return _filterResult;
}
