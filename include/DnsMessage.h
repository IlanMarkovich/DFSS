#pragma once

#include <vector>
#include <string>

#define byte unsigned char
#define bytes std::vector<byte>

using std::vector;
using std::string;

struct Query
{
    string name;
    int queryType;
    int dnsClass;
};

class DnsMessage
{
private:
    // Fields
    bytes _transactionId;
    bytes _flags;
    int _questions;
    int _answers_RRs;
    int _authority_RRs;
    int _additional_RRs;
    vector<Query> _queries;

public:
    // C'tor
    DnsMessage(const bytes& originalMessage);

    // Getters
    bytes getTransactionId() const;
    bytes getFlags() const;
    int getQuestions() const;
    int getAnswers_RRs() const;
    int getAuthority_RRs() const;
    int getAdditional_RRs() const;
    vector<Query> getQueries() const;
};