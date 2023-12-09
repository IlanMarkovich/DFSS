#include "DnsMessage.h"

// C'tor
DnsMessage::DnsMessage(const std::vector<unsigned char>& message)
{
    int i = 0;
    
    // Read all fields from the message given as input
    _transactionId = readPortionFromMessage(message, i);
    _flags = readPortionFromMessage(message, i);
    _questions = ByteHelper::bytesToInt(readPortionFromMessage(message, i));
    _answers_RRs = ByteHelper::bytesToInt(readPortionFromMessage(message, i));
    _authority_RRs = ByteHelper::bytesToInt(readPortionFromMessage(message, i));
    _additional_RRs = ByteHelper::bytesToInt(readPortionFromMessage(message, i));

    string queryName;
    i++;

    // Read query name from the queries
    while(message[i] != '\0')
    {
        // If the char is not a displayable character, insert a dot
        queryName += message[i] <= '\31' ? '.' : message[i];
        i++;
    }

    std::vector<unsigned char> type = readPortionFromMessage(message, ++i);
    std::vector<unsigned char> queryClass = readPortionFromMessage(message, i);

    _query = (Query){ queryName, ByteHelper::bytesToInt(type), queryClass };
}

// Getters

std::vector<unsigned char> DnsMessage::getTransactionId() const
{
    return _transactionId;
}

std::vector<unsigned char> DnsMessage::getFlags() const
{
    return _flags;
}

int DnsMessage::getQuestions() const
{
    return _questions;
}

int DnsMessage::getAnswers_RRs() const
{
    return _answers_RRs;
}

int DnsMessage::getAuthority_RRs() const
{
    return _authority_RRs;
}

int DnsMessage::getAdditional_RRs() const
{
    return _additional_RRs;
}

Query DnsMessage::getQuery() const
{
    return _query;
}

// Private Methods

std::vector<unsigned char> DnsMessage::readPortionFromMessage(const std::vector<unsigned char>& message, int& i)
{
    int destIndex = i + DNS_PROPERTY_SIZE;
    std::vector<unsigned char> out;

    for(; i < destIndex; i++)
    {
        out.push_back(message[i]);
    }

    return out;
}
