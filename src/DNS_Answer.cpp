#include "DNS_Answer.h"

// C'tor
DNS_Answer::DNS_Answer(const std::string & name, const std::vector<unsigned char>& dnsMsg, int & index)
: _name(name)
{
    const int TTL_SIZE = 4;

    // Skip the name because already got it
    index++;

    _type = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index));
    _answer_class = DNS_Reader::readPortionFromMessage(dnsMsg, index).data();
    _ttl = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, TTL_SIZE));
    _data_len = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index));
}
