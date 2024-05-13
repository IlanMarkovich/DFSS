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
    for(int ans = 0; ans < _answers_RRs + _authority_RRs; ans++)
    {
        // Skip the name and get the answer type
        index += _query.name == "" ? _query.name.size() + 1 : DNS_PROPERTY_SIZE;
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
                answer = new DNS_DNSKEY_Answer(type, message, index, domainToBuffer(queryName));
                break;
            case DNS_DS:
                answer = new DNS_DS_Answer(type, message, index);
                break;
            default:
                _DNSSEC_response = false;
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

DNS_Answer * DnsMessage::getData_RR() const
{
    // Checks if an answer is DS or A answer type, and if so returns it
    for(const auto& answer : _answers)
    {
        if(answer == nullptr)
            continue;

        switch(answer->getType())
        {
            case DNS_DS:
                return dynamic_cast<DNS_DS_Answer*>(answer);

            case DNS_A:
                return dynamic_cast<DNS_A_Answer*>(answer);
        }
    }

    return nullptr;
}

void DnsMessage::addOPT()
{
    const std::vector<unsigned char> OPT = {0x00, 0x00, 0x29, 0x10, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00};
    _messageInBytes.insert(_messageInBytes.end(), OPT.begin(), OPT.end());

    const int ADDITIONAL_RRs_INDEX = 11;
    _messageInBytes[ADDITIONAL_RRs_INDEX] = ++_additional_RRs;
}

void DnsMessage::changeQueryName(const std::string & newQueryName)
{
    int countName = _query.name.size();
    _query.name = newQueryName;
    
    const int QUERY_NAME_START_INDEX = 12;
    _messageInBytes.erase(_messageInBytes.begin() + QUERY_NAME_START_INDEX, _messageInBytes.begin() + QUERY_NAME_START_INDEX + countName + 1);

    std::vector<unsigned char> newQueryNameBuffer = domainToBuffer(newQueryName);

    if(countName == 0)
        newQueryNameBuffer.push_back('\0');

    _messageInBytes.insert(_messageInBytes.begin() + QUERY_NAME_START_INDEX, newQueryNameBuffer.begin(), newQueryNameBuffer.end());
}

void DnsMessage::changeQueryNameToRoot()
{
    int countTLD = _query.name.size() + 1;
    _query.name = "";

    // Deletes the TLD from the message in bytes, and put a null character so ROOT will be the query name
    const int QUERY_NAME_START_INDEX = 13;
    _messageInBytes.erase(_messageInBytes.begin() + QUERY_NAME_START_INDEX, _messageInBytes.begin() + QUERY_NAME_START_INDEX + countTLD);
    _messageInBytes[QUERY_NAME_START_INDEX - 1] = '\0';
}

void DnsMessage::changeMessageQueryType(int type)
{
    _query.type = type;

    // 13 is the index where the query name starts, the first 1 is to skip the null char (if not root)
    // and second 1 is to get to the second byte of the type to change it
    const int QUERY_TYPE_INDEX = 13 + _query.name.size() + (_query.name != "") + 1;
    _messageInBytes[QUERY_TYPE_INDEX] = type;
}

std::string DnsMessage::getTLD(const std::string & domain)
{
    int index = 0;

    while(std::count(domain.begin() + index, domain.end(), '.') != 0)
        index++;

    return std::string(domain.begin() + index, domain.end());
}

// Private Methods

std::vector<unsigned char> DnsMessage::domainToBuffer(const std::string & domain) const
{
    std::vector<unsigned char> domainBuffer;
    std::vector<unsigned char> buffer;

    for(unsigned int i = 0; i <= domain.size(); i++)
    {
        if(i == domain.size() || domain[i] == '.')
        {
            buffer.insert(buffer.begin(), (unsigned char)buffer.size());
            domainBuffer.insert(domainBuffer.end(), buffer.begin(), buffer.end());
            buffer = std::vector<unsigned char>();
            continue;
        }

        buffer.push_back(domain[i]);
    }

    return domainBuffer;
}
