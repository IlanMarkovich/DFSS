#include "DnsMessage.h"

// C'tor
DnsMessage::DnsMessage(const std::vector<unsigned char>& message)
: _messageInBytes(message)
{
    int index = 0;
    
    // Read all fields from the message given as input
    _transactionId = DNS_Reader::readPortionFromMessage(message, index);
    _flags = DNS_Reader::readPortionFromMessage(message, index);
    _queries = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, index));
    _answers_RRs = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, index));
    _authority_RRs = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, index));
    _additional_RRs = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, index));

    string queryName = DNS_Reader::readStringFromMessage(message, index);

    std::vector<unsigned char> type = DNS_Reader::readPortionFromMessage(message, index);
    std::vector<unsigned char> queryClass = DNS_Reader::readPortionFromMessage(message, index);

    _query = (Query){ queryName, ByteHelper::bytesToInt(type), queryClass.data() };

    // Read answers
    for(int ans = 0; ans < _answers_RRs; ans++)
    {
        string answerName = reinterpret_cast<const char*>(DNS_Reader::readPortionFromMessage(message, index).data());

        // Get the answer type
        int type = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, index));
        index -= DNS_PROPERTY_SIZE;

        DNS_Answer* answer = nullptr;

        switch(type)
        {
            case DNS_A:
                answer = new DNS_A_Answer(answerName, message, index);
                break;
            case DNS_RRSIG:
                answer = new DNS_RRSIG_Answer(answerName, message, index);
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

Query DnsMessage::getQuery() const
{
    return _query;
}

std::vector<unsigned char> DnsMessage::getMessageInBytes() const
{
    return _messageInBytes;
}

// Public Methods

void DnsMessage::addOPT()
{
    const unsigned char* OPT = reinterpret_cast<const unsigned char*>("\x00\x00\x29\x10\x00\x00\x00\x80\x00\x00\x00");
    _messageInBytes.insert(_messageInBytes.end(), OPT, OPT + sizeof(OPT));

    const int ADDITIONAL_RRs_INDEX = 11;
    _messageInBytes[ADDITIONAL_RRs_INDEX] = '\x01';
}
