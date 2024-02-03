#pragma once

#include "DnsMessage.hpp"
#include "Communicator.h"

#include <map>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/bn.h>

#define RSASHA1 5
#define RSASHA256 8
#define RSASHA512 10
#define ECDSAP256SHA256 13
#define ECDSAP384SHA384 14


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

    /// @brief Verify a DNS server by taking a DNSKEY response and a data response (DS or A)
    /// and do cryptographic validation checks with their digital signatures for authentication check
    /// @param dnskey_response Contains the DNSKEYS (KSK and ZSK) used for the cryptographic checks
    /// @param data_response Contains some sort of data which the DNS server send (A for ipv4 response or Dlegetion Signer for authentication of child zones)
    /// @param ds Contains the digest of the public KSK from the parent zone. Used to verify the public KSK
    /// @return Was the verification successful
    bool verifyServer(const DnsMessage* dnskey_response, const DnsMessage* data_response, const std::vector<unsigned char>& ds = std::vector<unsigned char>()) const;

    /// @brief Converts a DNSKEY (given its hashing function) to a digest (which is supposed to be the DS of the parent server)
    /// @param dnsKey The DNSKEY being converted (it's digest format: owner | rdata)
    /// @param hashFunction The hash function of the algorithm
    /// @return The convertion of the DNSKEY
    std::vector<unsigned char> toDS(const std::vector<unsigned char>& dnsKey, const EVP_MD* hashFunction) const;

    /// @brief Verify given its Public Key (KSK if data is key, ZSK if not), and a digital signature
    /// which is the result of encrypting the digest of the data using the DNS server's private key
    /// @param data The data being verified
    /// @param key The public key used for the decryption of the signature
    /// @param signature The signature used for the authentication of the data
    /// @return Was the verification of the data successful
    bool verifyData(const DNS_Answer* data, const DNS_DNSKEY_Answer& key, const DNS_RRSIG_Answer& signature) const;

    /// @brief Returns the public RSA key from a bytes buffer
    /// @param dnsKey The bytes buffer being converted to an RSA public key
    /// @return The public RSA key
    RSA* publicKeyRSA(const std::vector<unsigned char>& dnsKey) const;

    /// @brief Returns an ECDSA public key from a bytes buffer
    /// @param dnsKey The bytes buffer being converted to an ECDSA public key
    /// @param algorithm The algorithm of the ECDSA (P256 or P384)
    /// @return The public ECDSA key
    EC_KEY* publicKeyECDSA(const std::vector<unsigned char>& dnsKey, int algorithm) const;

    /// @brief Verifies a buffer of data using RSA and a certain hashing method
    /// @param key The RSA public key used for the decryption of the digital signature
    /// @param data The data being verified
    /// @param signature The digital signature which represents the data
    /// @param hashFunction The hashing method which is being used to get the digest of the data
    /// @return The result of the verification
    bool verifyRSA(RSA* key, const std::vector<unsigned char>& data, const std::vector<unsigned char>& signature, const EVP_MD* hashFunction) const;

    /// @brief Verifies a buffer of data using ECDSA
    /// @param key The ECDSA public key used for the decryption of the digital signature
    /// @param data The data begin verified
    /// @param r,s Two BIGNUM types which represent the digital signature
    /// @param algorithm The algorithm of the ECDSA (P256 or P384)
    /// @return The result of the verification
    bool verifyECDSA(EC_KEY* key, const std::vector<unsigned char>& data, const BIGNUM* r, const BIGNUM* s, int algorithm) const;
};
