#include "DnsMessage.hpp"

#include <algorithm>

// C'tor
DnsMessage::DnsMessage(const std::vector<unsigned char>& message)
: _messageInBytes(message), _DNSSEC_response(false)
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
        // Skip the name and get the answer type
        index += DNS_PROPERTY_SIZE;
        int type = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(message, index));

        DNS_Answer* answer = nullptr;

        switch(type)
        {
            case DNS_A:
                answer = new DNS_A_Answer(type, message, index);
                break;
            case DNS_RRSIG:
                answer = new DNS_RRSIG_Answer(type, message, index);
                _DNSSEC_response = true;
                break;
            case DNS_DNSKEY:
                answer = new DNS_DNSKEY_Answer(type, message, index);
                break;
            case DNS_DS:
                answer = new DNS_DS_Answer(type, message, index);
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

bool DnsMessage::is_DNSSEC_response() const
{
    return _DNSSEC_response;
}

// Public Methods

void DnsMessage::addOPT()
{
    const std::vector<unsigned char> OPT = {0x00, 0x00, 0x29, 0x10, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00};
    _messageInBytes.insert(_messageInBytes.end(), OPT.begin(), OPT.end());

    const int ADDITIONAL_RRs_INDEX = 11;
    _messageInBytes[ADDITIONAL_RRs_INDEX] = ++_additional_RRs;
}

void DnsMessage::changeToTLD()
{
    // ---------- Change the query name field ----------
    int countName = 1;
    auto iter = _query.name.begin();

    while(*iter != '.' || std::count(iter + 1, _query.name.end(), '.') != 0)
    {
        iter++;
        countName++;
    }

    _query.name = std::string(iter + 1, _query.name.end());

    // ---------- Change the query name in `_messageInBytes` ----------
    const int QUERY_NAME_START_INDEX = 13;
    _messageInBytes.erase(_messageInBytes.begin() + QUERY_NAME_START_INDEX, _messageInBytes.begin() + QUERY_NAME_START_INDEX + countName);
}

void DnsMessage::changeMessageQueryType(int type)
{
    _query.type = type;

    // 13 is the index where the query name starts, the first 1 is to skip the null char
    // and second 1 is to get to the second byte of the type to change it
    const int QUERY_TYPE_INDEX = 13 + _query.name.size() + 1 + 1;
    _messageInBytes[QUERY_TYPE_INDEX] = type;
}
