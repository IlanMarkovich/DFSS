#pragma once

#include <string>
#include <vector>

#include "DNS_Reader.h"
#include "ByteHelper.h"


class DNS_Answer
{
protected:
    // Fields
    int _type;
    const unsigned char* _answer_class;
    int _ttl;
    int _data_len;

public:
    // C'tor
    DNS_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);

    // D'tor
    virtual ~DNS_Answer();

    // Getters

    int getType() const;

    // Methods

    virtual std::vector<unsigned char> getData() const = 0;
};
