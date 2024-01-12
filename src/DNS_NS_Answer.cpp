#include "DNS_NS_Answer.h"

// C'tor
DNS_NS_Answer::DNS_NS_Answer(int type, const std::vector<unsigned char>& dnsMsg, int & index)
: DNS_Answer(type, dnsMsg, index)
{
    _name_server = DNS_Reader::readStringFromMessage(dnsMsg, index);
}

// Getters

std::string DNS_NS_Answer::getNameServer() const
{
    return _name_server;
}
