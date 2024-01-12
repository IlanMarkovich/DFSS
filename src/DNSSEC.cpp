#include "DNSSEC.h"

#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

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

    std::string queryName = _request.getQuery().name;

    _request.changeQueryNameToRoot();
    DnsMessage root_A_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    _request.changeQueryName(queryName);
    _request.addOPT();
    DnsMessage domain_A_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    DnsMessage* domain_DNSKEY_response = nullptr;
    if(domain_A_response.is_DNSSEC_response())
    {
        _request.changeMessageQueryType(DNS_DNSKEY);
        domain_DNSKEY_response = new DnsMessage(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    }

    _request.changeMessageQueryType(DNS_DS);
    std::string root_domain = root_A_response.getResponse_RRset<DNS_SOA_Answer>()[0].getNameServer();
    DnsMessage root_TLD_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes(), Communicator::getDomainIP(root_domain).c_str()));
    std::string TLD_domain = root_TLD_response.getResponse_RRset<DNS_NS_Answer>()[0].getNameServer();
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
