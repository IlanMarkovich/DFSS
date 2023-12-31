#pragma once

#include <string>

#include "ByteHelper.h"
#include "DNS_Reader.h"
#include "DNS_A_Answer.h"
#include "DNS_RRSIG_Answer.h"

#define DNS_PROPERTY_SIZE 2
#define DNS_A 1
#define DNS_RRSIG 46

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
    std::vector<unsigned char> _transactionId;
    std::vector<unsigned char> _flags;
    int _queries;
    int _answers_RRs;
    int _authority_RRs;
    int _additional_RRs;
    Query _query;
    std::vector<DNS_Answer*> _answers;

public:
    // C'tor
    DnsMessage(const std::vector<unsigned char>& message);

    // D'tor
    ~DnsMessage();

    // Getters

    std::vector<unsigned char> getTransactionId() const;
    std::vector<unsigned char> getFlags() const;
    int getQueries() const;
    int getAnswers_RRs() const;
    int getAuthority_RRs() const;
    int getAdditional_RRs() const;
    Query getQuery() const;
};