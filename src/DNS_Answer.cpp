#include "DNS_Answer.h"

// C'tor
DNS_Answer::DNS_Answer(int type, const std::vector<unsigned char>& dnsMsg, int & index)
: _type(type)
{
    const int TTL_SIZE = 4;

    _answer_class = DNS_Reader::readPortionFromMessage(dnsMsg, index).data();
    _ttl = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, TTL_SIZE));
    _data_len = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index));
}

// D'tor
DNS_Answer::~DNS_Answer() {}

// Getters

int DNS_Answer::getType() const
{
    return _type;
}
