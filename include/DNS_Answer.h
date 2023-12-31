#pragma once

#include <string>
#include <vector>

#include "DNS_Reader.h"
#include "ByteHelper.h"


class DNS_Answer
{
protected:
    // Fields
    std::string _name;
    int _type;
    const unsigned char* _answer_class;
    int _ttl;
    int _data_len;

public:
    // C'tor
    DNS_Answer(const std::string& name, const std::vector<unsigned char>& dnsMsg, int& index);
};
