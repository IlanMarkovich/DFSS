#pragma once

#include <string>

#include "ByteHelper.h"
#include "DNS_Answer.h"
#include "DNS_A_Answer.h"
#include "DNS_RRSIG_Answer.h"

#define DNS_PROPERTY_SIZE 2
#define DNS_A 1
#define DNS_RRSIG 46

using std::string;

class DNS_Answer;


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

    // Methods

    /// @brief Reads a portion of a bytes sequence (reffered as message) from a certain index
    /// @param message The sequence of bytes being read from
    /// @param i The index which the function starts to read from
    /// @param prop_size The property size to calculate the destination index (equals to DNS_PROPERTY_SIZE on default)
    /// @return The new sequence read from the message
    static std::vector<unsigned char> readPortionFromMessage(const std::vector<unsigned char>& message, int& i, int prop_size = DNS_PROPERTY_SIZE);

    /// @brief Read a certain string from a bytes sequence which represents a DNS message
    /// @param message The DNS message in bytes
    /// @param i The start index of the string
    /// @return The string read from the message
    static std::string readStringFromMessage(const std::vector<unsigned char>& message, int& i);
};