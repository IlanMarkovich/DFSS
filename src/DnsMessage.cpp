#include "DnsMessage.h"

// C'tor
DnsMessage::DnsMessage(const bytes& message)
{
    int i = 0;
    
    // Read all fields from the message given as input
    _transactionId = readPortionFromMessage(message, i);
    _flags = readPortionFromMessage(message, i);
    _questions = readPortionFromMessage(message, i);
    _answers_RRs = readPortionFromMessage(message, i);
    _authority_RRs = readPortionFromMessage(message, i);
    _additional_RRs = readPortionFromMessage(message, i);

    string queryName;

    // Read query name from the queries
    while(message[i] != '\0')
    {
        queryName += message[i++];
    }

    bytes queryType = readPortionFromMessage(message, ++i);
    bytes dnsClass = readPortionFromMessage(message, i);

    _query = (Query){ queryName, ByteHelper::bytesToInt(queryType), dnsClass };
}

// Getters

bytes DnsMessage::getTransactionId() const
{
    return _transactionId;
}

bytes DnsMessage::getFlags() const
{
    return _flags;
}

int DnsMessage::getQuestions() const
{
    return ByteHelper::bytesToInt(_questions);
}

int DnsMessage::getAnswers_RRs() const
{
    return ByteHelper::bytesToInt(_answers_RRs);
}

int DnsMessage::getAuthority_RRs() const
{
    return ByteHelper::bytesToInt(_authority_RRs);
}

int DnsMessage::getAdditional_RRs() const
{
    return ByteHelper::bytesToInt(_additional_RRs);
}

Query DnsMessage::getQuery() const
{
    return _query;
}

// Private Methods

bytes DnsMessage::readPortionFromMessage(const bytes& message, int& i)
{
    int destIndex = i + DNS_PROPERTY_SIZE;
    bytes out;

    for(; i < destIndex; i++)
    {
        out.push_back(message[i]);
    }

    return out;
}
