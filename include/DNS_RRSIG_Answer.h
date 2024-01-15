#pragma once

#include "DNS_Answer.h"


class DNS_RRSIG_Answer : public DNS_Answer
{
private:
    // Fields
    int _type_covered;
    int _algorithm;
    int _labels;
    int _original_ttl;
    const unsigned char* _sig_expiration;
    const unsigned char* _sig_inception;
    int _key_tag;
    std::string _signer_name;
    std::vector<unsigned char> _signature;

public:
    // C'tor
    DNS_RRSIG_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);

    // Getters

    int getTypeCovered() const;
    int getAlgorithm() const;
    int getKeyTag() const;
    std::vector<unsigned char> getSignature() const;
};
