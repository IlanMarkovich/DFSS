#pragma once

#include <string>

#include "ByteHelper.h"

#define DNS_PROPERTY_SIZE 2

using std::string;

struct Query
{
    string name;
    int type;
};

class DnsMessage
{
private:
    // Fields
    std::vector<unsigned char> _transactionId;
    std::vector<unsigned char> _flags;
    int _questions;
    int _answers_RRs;
    int _authority_RRs;
    int _additional_RRs;
    Query _query;

public:
    // C'tor
    DnsMessage(const std::vector<unsigned char>& message);

    // Getters
    std::vector<unsigned char> getTransactionId() const;
    std::vector<unsigned char> getFlags() const;
    int getQuestions() const;
    int getAnswers_RRs() const;
    int getAuthority_RRs() const;
    int getAdditional_RRs() const;
    Query getQuery() const;

private:
    // Methods

    /// @brief Reads a portion of a bytes sequence (reffered as message) from a certain index
    /// @param message The sequence of bytes being read from
    /// @param i The index which the function starts to read from
    /// @return The new sequence read from the message
    std::vector<unsigned char> readPortionFromMessage(const std::vector<unsigned char>& message, int& i);
};