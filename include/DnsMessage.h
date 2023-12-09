#pragma once

#include <string>

#include "ByteHelper.h"

#define DNS_PROPERTY_SIZE 2

using std::string;

struct Query
{
    string name;
    int type;
    bytes queryClass;
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
    Query _query;

public:
    // C'tor
    DnsMessage(const bytes& message);

    // Getters
    bytes getTransactionId() const;
    bytes getFlags() const;
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
    bytes readPortionFromMessage(const bytes& message, int& i);
};