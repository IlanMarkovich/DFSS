#include "DNS_NS_Answer.h"

// C'tor
DNS_NS_Answer::DNS_NS_Answer(int type, const std::vector<unsigned char>& dnsMsg, int & index)
: DNS_Answer(type, dnsMsg, index)
{
    _name_server = DNS_Reader::readPortionFromMessage(dnsMsg, ++index, _data_len - 1);

    for(unsigned int i = 0; i < _name_server.size(); i++)
    {
        if(_name_server[i] <= '\31')
            _name_server[i] = '.';
    }

    // Delete null char
    _name_server.erase(_name_server.end() - 1, _name_server.end());
}

// Getters

std::string DNS_NS_Answer::getNameServer() const
{
    return std::string(_name_server.begin(), _name_server.end());
}
