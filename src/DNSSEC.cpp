#include "DNSSEC.h"

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    // --------------- GET DNSSEC RESPONSES FROM SERVERS ---------------
    // _request.addOPT();
    // DnsMessage domain_A_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    // DnsMessage* domain_DNSKEY_response = nullptr;

    // if(domain_A_response.is_DNSSEC_response())
    // {
    //     _request.changeMessageQueryType(DNS_DNSKEY);
    //     domain_DNSKEY_response = new DnsMessage(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    // }

    // _request.changeToTLD();
    // _request.changeMessageQueryType(DNS_DNSKEY);
    // DnsMessage TLD_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    // _request.changeMessageQueryType(DNS_DS);
    // DnsMessage TLD_DS_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    // _request.changeToRoot();
    // _request.changeMessageQueryType(DNS_DNSKEY);
    // DnsMessage ROOT_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    // _request.changeMessageQueryType(DNS_DS);
    // DnsMessage ROOT_DS_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    // --------------- --------------- --------------- ---------------

    _request.changeToRoot();
    Communicator::DNS_ResponseFetcher(_request.getMessageInBytes());
}

// Getters

DNSSEC_Level DNSSEC::getFilterResult() const
{
    return _filterResult;
}

// Private Methods

bool DNSSEC::validateServer(DnsMessage DNSKEY_response, DnsMessage data_response, std::vector<unsigned char> KSK_verificatione)
{
    return true;
}
