#include "DNS_SOA_Answer.h"

// C'tor
DNS_SOA_Answer::DNS_SOA_Answer(int type, const std::vector<unsigned char>& dnsMsg, int & index)
: DNS_Answer(type, dnsMsg, index)
{
    _primary_name_server = DNS_Reader::readStringFromMessage(dnsMsg, index);
    _authority_mailbox = DNS_Reader::readStringFromMessage(dnsMsg, index);

    const int SERIAL_NUMBER_SIZE = 4;
    _serial_number = DNS_Reader::readPortionFromMessage(dnsMsg, index, SERIAL_NUMBER_SIZE);

    const int TIME_SIZE = 4;
    _refresh_interval = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, TIME_SIZE));
    _retry_interval = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, TIME_SIZE));
    _expire_limit = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, TIME_SIZE));
    _minimum_ttl = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, TIME_SIZE));
}

// Getters

std::string DNS_SOA_Answer::getNameServer() const
{
    return _primary_name_server;
}
