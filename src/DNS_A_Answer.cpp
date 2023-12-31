#include "DNS_A_Answer.h"

// C'tor
DNS_A_Answer::DNS_A_Answer(const std::string& name, const std::vector<unsigned char>& dnsMsg, int& index)
: DNS_Answer(name, dnsMsg, index)
{
    for(int i = 0; i < _data_len; i++)
    {
        unsigned char c = DnsMessage::readPortionFromMessage(dnsMsg, index, 1)[0];
        _ip_address += c + '.';
    }

    // Remove the last '.'
    _ip_address.pop_back();
}
