#pragma once

#include "DNS_Answer.h"


class DNS_DNSKEY_Answer : public DNS_Answer
{
private:
    // Fields

    std::vector<unsigned char> _flags;
    int _protocol;
    int _algorithm;
    std::vector<unsigned char> _public_key;

public:
    // C'tor
    DNS_DNSKEY_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index);

    // Getters

    int getAlgorithm() const;
    std::vector<unsigned char> getPublicKey() const;

    // Methods

    /// @brief Gets the KSK of a given vector of dnskeys
    /// @param keys The vector of dnskeys where the KSK is located
    /// @return The requested KSKS
    static DNS_DNSKEY_Answer extractKSK(const std::vector<DNS_DNSKEY_Answer>& keys);

    virtual std::vector<unsigned char> getData() const override;
};
