#include "DNS_A_Answer.h"

// C'tor
DNS_A_Answer::DNS_A_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index)
: DNS_Answer(type, dnsMsg, index)
{
    const int IP_ADDRESS_SIZE = 4;
    _ip_address = DNS_Reader::readPortionFromMessage(dnsMsg, index, IP_ADDRESS_SIZE);
}

// Getters 

std::vector<unsigned char> DNS_A_Answer::get_IP_address() const
{
    return _ip_address;
}
