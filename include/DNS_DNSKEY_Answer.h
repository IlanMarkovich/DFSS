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
    
    std::vector<unsigned char> _owner;
    std::vector<unsigned char> _rdata;

public:
    // C'tor
    DNS_DNSKEY_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index, const std::vector<unsigned char>& owner);

    // Getters

    int getAlgorithm() const;
    std::vector<unsigned char> getPublicKey() const;

    // Methods

    virtual std::vector<unsigned char> getData() const override;

    /// @brief Gets the KSK of a given vector of dnskeys
    /// @param keys The vector of dnskeys where the KSK is located
    /// @return The requested KSKS
    static DNS_DNSKEY_Answer extractKSK(const std::vector<DNS_DNSKEY_Answer>& keys);

    /// @brief Returns the concatanation of the owner and rdata of the DNSKEY
    /// for it to be hashed correctly
    std::vector<unsigned char> getDigestFormat() const;
};
