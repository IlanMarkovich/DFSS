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
    bytes dnsClass = readPortionFromMessage(message, ++i);

    _query = (Query){ queryName, ByteHelper::bytesToInt(queryType), dnsClass };
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
