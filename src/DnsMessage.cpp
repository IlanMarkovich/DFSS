#include "DnsMessage.h"

// C'tor
DnsMessage::DnsMessage(const std::vector<unsigned char>& message)
{
    int i = 0;
    
    // Read all fields from the message given as input
    _transactionId = DNS_Reader::readPortionFromMessage(message, i);
    _flags = DNS_Reader::readPortionFromMessage(message, i);
    _queries = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, i));
    _answers_RRs = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, i));
    _authority_RRs = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, i));
    _additional_RRs = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, i));

    string queryName = DNS_Reader::readStringFromMessage(message, i);

    std::vector<unsigned char> type = DNS_Reader::readPortionFromMessage(message, i);
    std::vector<unsigned char> queryClass = DNS_Reader::readPortionFromMessage(message, i);

    _query = (Query){ queryName, ByteHelper::bytesToInt(type), queryClass.data() };

    // Read answers
    for(int ans = 0; ans < _answers_RRs; ans++)
    {
        string answerName = reinterpret_cast<const char*>(DNS_Reader::readPortionFromMessage(message, i).data());

        // Get the answer type
        int type = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, i));
        i -= DNS_PROPERTY_SIZE;

        DNS_Answer* answer = nullptr;

        switch(type)
        {
            case DNS_A:
                answer = new DNS_A_Answer(answerName, message, i);
                break;
            case DNS_RRSIG:
                answer = new DNS_RRSIG_Answer(answerName, message, i);
                break;
        }

        _answers.push_back(answer);
    }
}

// D'tor
DnsMessage::~DnsMessage()
{
    for(auto& answer : _answers)
    {
        delete answer;
    }
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

int DnsMessage::getQueries() const
{
    return _queries;
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
