#pragma once

#include "DnsMessage.hpp"
#include "Communicator.h"

#include <map>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/bn.h>


enum DNSSEC_Level
{
    Non, Domain, TLD, Root
};

enum DNSSEC_Encryption
{
    DNSSEC_RSA, DNSSEC_ECDSA
};

class DNSSEC
{
private:
    // Fields

    DnsMessage _request;
    DNSSEC_Level _filterResult;
    static std::map<int, const EVP_MD*> algorithmToHash;
    static std::map<int, DNSSEC_Encryption> algorithmToEncrpytion;

public:
    // C'tor
    DNSSEC(const DnsMessage& request);

    // Getters

    DNSSEC_Level getFilterResult() const;

private:
    // Methods

    bool verifyServer(const DnsMessage* dnskey_response, const DnsMessage* data_response) const;

    bool verifyData(const DNS_Answer& data, const DNS_DNSKEY_Answer& key, const DNS_RRSIG_Answer& signature) const;

    RSA* publicKeyRSA(const std::vector<unsigned char>& dnsKey) const;

    EC_KEY* publicKeyECDSA(const std::vector<unsigned char>& dnsKey, int algorithm) const;

    bool verifyPKCS1v15(RSA* key, const std::vector<unsigned char>& encryptedData, const std::vector<unsigned char>& signature, const EVP_MD* hashFunction) const;

    bool verifyECDSA(EC_KEY* key, const std::vector<unsigned char>& data, const BIGNUM* r, const BIGNUM* s, const EVP_MD* hashFunction) const;
};
