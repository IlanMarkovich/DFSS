#include "DnsMessage.h"

// C'tor
DnsMessage::DnsMessage(const std::vector<unsigned char>& message)
{
    int i = 0;
    
    // Read all fields from the message given as input
    _transactionId = readPortionFromMessage(message, i);
    _flags = readPortionFromMessage(message, i);
    _queries = ByteHelper::bytesToInt(readPortionFromMessage(message, i));
    _answers_RRs = ByteHelper::bytesToInt(readPortionFromMessage(message, i));
    _authority_RRs = ByteHelper::bytesToInt(readPortionFromMessage(message, i));
    _additional_RRs = ByteHelper::bytesToInt(readPortionFromMessage(message, i));

    string queryName = readStringFromMessage(message, i);

    std::vector<unsigned char> type = readPortionFromMessage(message, i);
    std::vector<unsigned char> queryClass = readPortionFromMessage(message, i);

    _query = (Query){ queryName, ByteHelper::bytesToInt(type), queryClass.data() };

    // Read answers
    for(int ans = 0; ans < _answers_RRs; ans++)
    {
        string answerName = reinterpret_cast<const char*>(readPortionFromMessage(message, i).data());

        // Get the answer type
        int type = ByteHelper::bytesToInt(DnsMessage::readPortionFromMessage(message, i));
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

// Public Methods

std::vector<unsigned char> DnsMessage::readPortionFromMessage(const std::vector<unsigned char>& message, int& i, int prop_size)
{
    int destIndex = i + prop_size;
    std::vector<unsigned char> out;

    for(; i < destIndex; i++)
    {
        out.push_back(message[i]);
    }

    return out;
}

std::string DnsMessage::readStringFromMessage(const std::vector<unsigned char>& message, int & i)
{
    std::string str;

    for(i++ ; message[i] != '\0'; i++)
    {
        // If the char is not a displayable character, insert a dot
        str += message[i] <= '\31' ? '.' : message[i];
    }

    i++;
    return str;
}