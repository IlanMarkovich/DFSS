#pragma once

#include <string>

#include "ByteHelper.h"
#include "DNS_Reader.h"
#include "DNS_A_Answer.h"
#include "DNS_RRSIG_Answer.h"
#include "DNS_DNSKEY_Answer.h"

#define DNS_PROPERTY_SIZE 2
#define DNS_A 1
#define DNS_RRSIG 46
#define DNS_DNSKEY 48

using std::string;


struct Query
{
    string name;
    int type;
    const unsigned char* queryClass;
};

class DnsMessage
{
private:
    // Fields

    std::vector<unsigned char> _messageInBytes;
    std::vector<unsigned char> _transactionId;
    std::vector<unsigned char> _flags;
    int _queries;
    int _answers_RRs;
    int _authority_RRs;
    int _additional_RRs;
    Query _query;
    std::vector<DNS_Answer*> _answers;
    bool _DNSSEC_response;

public:
    // C'tor
    DnsMessage(const std::vector<unsigned char>& message);

    // D'tor
    ~DnsMessage();

    // Getters

    Query getQuery() const;
    std::vector<unsigned char> getMessageInBytes() const;
    bool is_DNSSEC_response() const;

    // Methods

    /// @brief Adds the additional RR called OPT with the DO flag to
    /// tell the DNS resolver to use DNSSEC
    void addOPT();

    /// @brief Change the query name to only the TLD name
    void changeToTLD();

    /// @brief Changes the query type to DNSKEY
    void requestDNSKEY();
};