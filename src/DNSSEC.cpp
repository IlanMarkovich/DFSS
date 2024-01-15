#include "DNSSEC.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// Static variables initialization

std::map<int, const EVP_MD*> DNSSEC::algorithmToHash = {
    {5, EVP_sha1()},
    {7, EVP_sha1()},
    {8, EVP_sha256()},
    {10, EVP_sha512()},
    {13, EVP_sha256()},
    {14, EVP_sha384()}
};

std::map<int, DNSSEC_Encryption> DNSSEC::algorithmToEncrpytion = {
    {5, DNSSEC_RSA},
    {7, DNSSEC_RSA},
    {8, DNSSEC_RSA},
    {10, DNSSEC_RSA},
    {13, DNSSEC_ECDSA},
    {14, DNSSEC_ECDSA}
};

// C'tor
DNSSEC::DNSSEC(const DnsMessage & request)
: _request(request)
{
    std::string queryName = _request.getQuery().name;
    
    // Get the DNSSEC response about the full domain (and DS from TLD if supports DNSSEC)
    _request.addOPT();
    _request.changeMessageQueryType(DNS_A);
    DnsMessage domain_A_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

    DnsMessage* domain_DNSKEY_response = nullptr;
    DnsMessage* TLD_DS_response = nullptr;

    if(domain_A_response.is_DNSSEC_response())
    {
        _request.changeMessageQueryType(DNS_DNSKEY);
        domain_DNSKEY_response = new DnsMessage(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));

        _request.changeMessageQueryType(DNS_DS);
        TLD_DS_response = new DnsMessage(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    }
    // ------------------------------------------------------------------------------------------

    // Get the DNSSEC response from the TLD
    _request.changeMessageQueryType(DNS_DNSKEY);
    _request.changeQueryName(DnsMessage::getTLD(queryName));
    DnsMessage TLD_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    // ------------------------------------------------------------------------------------------

    // Get the DNSSEC response from the Root
    _request.changeMessageQueryType(DNS_DS);
    DnsMessage root_DS_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    _request.changeMessageQueryType(DNS_DNSKEY);
    _request.changeQueryNameToRoot();
    DnsMessage root_DNSKEY_response(Communicator::DNS_ResponseFetcher(_request.getMessageInBytes()));
    // ------------------------------------------------------------------------------------------

    // Checking the recieved data with their digital signatures
    _filterResult = verifyServer(&root_DNSKEY_response, &root_DS_response) ? Root : Non;
    _filterResult = _filterResult == Root && verifyServer(&TLD_DNSKEY_response, TLD_DS_response) ? TLD : Root;
    _filterResult = _filterResult == TLD && verifyServer(domain_DNSKEY_response, &domain_A_response) ? Domain : TLD;

    if(domain_A_response.is_DNSSEC_response())
    {
        delete domain_DNSKEY_response;
        delete TLD_DS_response;
    }
}

// Getters

DNSSEC_Level DNSSEC::getFilterResult() const
{
    return _filterResult;
}

// Private Methods

bool DNSSEC::verifyServer(const DnsMessage * dnskey_response, const DnsMessage * data_response) const
{
    if(dnskey_response == nullptr)
        return false;

    std::vector<DNS_DNSKEY_Answer> keys = dnskey_response->getResponse_RRset<DNS_DNSKEY_Answer>();
    std::vector<DNS_RRSIG_Answer> keys_sigs = dnskey_response->getResponse_RRset<DNS_RRSIG_Answer>();
    DNS_DNSKEY_Answer ksk = DNS_DNSKEY_Answer::extractKSK(keys);

    for(const auto& sig : keys_sigs)
    {
        if(sig.getTypeCovered() != DNS_DNSKEY)
            return false;

        if(sig.getAlgorithm() != ksk.getAlgorithm())
            return false;

        if(!verifyData(&ksk, ksk, sig))
            return false;
    }

    if(data_response == nullptr)
        return true;

    DNS_Answer* data = data_response->getData_RR();
    DNS_RRSIG_Answer data_sig = data_response->getResponse_RRset<DNS_RRSIG_Answer>()[0];

    for(const auto& key : keys)
    {
        if(!verifyData(data, key, data_sig))
            return false;
    }

    return true;
}

bool DNSSEC::verifyData(const DNS_Answer * data, const DNS_DNSKEY_Answer & key, const DNS_RRSIG_Answer & signature) const
{
    int algorithm = key.getAlgorithm();
    const EVP_MD* hash = algorithmToHash.at(algorithm);
    DNSSEC_Encryption enc = algorithmToEncrpytion.at(algorithm);

    if(enc == DNSSEC_RSA)
    {
        RSA* rsaKey = publicKeyRSA(key.getPublicKey());

        if(rsaKey == nullptr)
            return false;
        
        return verifyRSA(rsaKey, data->getData(), signature.getSignature(), hash);
    }
    else if(enc == DNSSEC_ECDSA)
    {
        EC_KEY* ecKey = publicKeyECDSA(key.getPublicKey(), algorithm);
        auto sigbuf = signature.getSignature();

        size_t halfSize = sigbuf.size() / 2;

        BIGNUM* r = BN_bin2bn(sigbuf.data(), halfSize, nullptr);
        BIGNUM* s = BN_bin2bn(sigbuf.data() + halfSize, halfSize, nullptr);

        if (!r || !s) {
            BN_free(r);
            BN_free(s);
            return false;
        }

        return verifyECDSA(ecKey, data->getData(), r, s, algorithm);
    }

    return false;
}

RSA* DNSSEC::publicKeyRSA(const std::vector<unsigned char>& dnsKey) const
{
    if (dnsKey.size() < 1 + 1 + 64) {
        // Exponent must be at least 1 byte and modulus at least 64
        return nullptr;
    }

    // RFC 2537/3110, section 2. RSA Public KEY Resource Records
    // Length is in the 0th byte, unless its zero, then it
    // it in bytes 1 and 2 and its a 16 bit number
    uint16_t explen = dnsKey[0];
    size_t keyoff = 1;
    if (explen == 0) {
        explen = (dnsKey[1] << 8) | dnsKey[2];
        keyoff = 3;
    }

    if (explen > 4 || explen == 0 || dnsKey[keyoff] == 0) {
        // Exponent larger than supported by the crypto package,
        // empty, or contains prohibited leading zero.
        return nullptr;
    }

    size_t modoff = keyoff + explen;
    size_t modlen = dnsKey.size() - modoff;
    if (modlen < 64 || modlen > 512 || dnsKey[modoff] == 0) {
        // Modulus is too small, large, or contains prohibited leading zero.
        return nullptr;
    }

    RSA* rsa = RSA_new();
    BIGNUM* e = BN_new();
    BIGNUM* n = BN_new();

    // The exponent of length explen is between keyoff and modoff.
    for (size_t i = keyoff; i < modoff; ++i) {
        BN_lshift(e, e, 8);
        BN_add_word(e, dnsKey[i]);
    }

    if (BN_get_word(e) > 1<<(31-1)) {
        // Larger exponent than supported by the crypto package.
        RSA_free(rsa);
        BN_free(e);
        BN_free(n);
        return nullptr;
    }

    BN_bin2bn(&dnsKey[modoff], modlen, n);

    RSA_set0_key(rsa, n, e, nullptr);

    return rsa;
}

EC_KEY * DNSSEC::publicKeyECDSA(const std::vector<unsigned char>& dnsKey, int algorithm) const
{
    EC_KEY* ecKey = EC_KEY_new();
    if (ecKey == nullptr) {
        // Handle error
        return nullptr;
    }

    switch (algorithm) {
        case ECDSAP256SHA256: {
            EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
            if (group == nullptr) {
                // Handle error
                EC_KEY_free(ecKey);
                return nullptr;
            }
            EC_KEY_set_group(ecKey, group);
            EC_GROUP_free(group);

            if (dnsKey.size() != 64) {
                // Wrongly encoded key
                EC_KEY_free(ecKey);
                return nullptr;
            }
            break;
        }
        case ECDSAP384SHA384: {
            EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp384r1);
            if (group == nullptr) {
                // Handle error
                EC_KEY_free(ecKey);
                return nullptr;
            }
            EC_KEY_set_group(ecKey, group);
            EC_GROUP_free(group);

            if (dnsKey.size() != 96) {
                // Wrongly encoded key
                EC_KEY_free(ecKey);
                return nullptr;
            }
            break;
        }
        default:
            // Unsupported algorithm
            EC_KEY_free(ecKey);
            return nullptr;
    }

    BIGNUM* x = BN_bin2bn(dnsKey.data(), dnsKey.size() / 2, nullptr);
    BIGNUM* y = BN_bin2bn(dnsKey.data() + dnsKey.size() / 2, dnsKey.size() / 2, nullptr);
    if (x == nullptr || y == nullptr) {
        // Handle error
        BN_free(x);
        BN_free(y);
        EC_KEY_free(ecKey);
        return nullptr;
    }

    if (EC_KEY_set_public_key_affine_coordinates(ecKey, x, y) != 1) {
        // Handle error
        BN_free(x);
        BN_free(y);
        EC_KEY_free(ecKey);
        return nullptr;
    }

    BN_free(x);
    BN_free(y);

    return ecKey;
}

bool DNSSEC::verifyRSA(RSA * key, const std::vector<unsigned char>& encryptedData, const std::vector<unsigned char>& signature, const EVP_MD * hashFunction) const
{
    if (!key || encryptedData.empty() || signature.empty() || !hashFunction) {
        return false;
    }

    EVP_PKEY* evpKey = EVP_PKEY_new();
    if (!evpKey) {
        return false;
    }

    if (EVP_PKEY_set1_RSA(evpKey, key) != 1) {
        EVP_PKEY_free(evpKey);
        return false;
    }

    EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
    if (!mdCtx) {
        EVP_PKEY_free(evpKey);
        return false;
    }

    if (EVP_DigestVerifyInit(mdCtx, nullptr, hashFunction, nullptr, evpKey) != 1) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(evpKey);
        return false;
    }

    if (EVP_DigestVerifyUpdate(mdCtx, encryptedData.data(), encryptedData.size()) != 1) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(evpKey);
        return false;
    }

    EVP_DigestVerifyFinal(mdCtx, signature.data(), signature.size());

    EVP_MD_CTX_free(mdCtx);
    EVP_PKEY_free(evpKey);

    return true;
}

bool DNSSEC::verifyECDSA(EC_KEY * key, const std::vector<unsigned char>& data, const BIGNUM * r, const BIGNUM * s, int algorithm) const
{
    // Create an ECDSA_SIG structure from the provided r and s components
    ECDSA_SIG* signature = ECDSA_SIG_new();
    if (signature == nullptr) {
        // Handle memory allocation failure
        return false;
    }

    // Set r and s components of the signature
    if (!ECDSA_SIG_set0(signature, const_cast<BIGNUM*>(r), const_cast<BIGNUM*>(s))) {
        // Handle error in setting r and s components
        ECDSA_SIG_free(signature);
        return false;
    }

    switch(algorithm)
    {
        case ECDSAP256SHA256:
            // Calculate the SHA-256 hash of the data
            unsigned char hash_sha256[SHA256_DIGEST_LENGTH];
            SHA256(&data[0], data.size(), hash_sha256);

            // Verify the ECDSA signature
            ECDSA_do_verify(hash_sha256, SHA256_DIGEST_LENGTH, signature, key);
            break;
        
        case ECDSAP384SHA384:
            // Calculate the SHA-384 hash of the data
            unsigned char hash_sha384[SHA384_DIGEST_LENGTH];
            SHA384(data.data(), data.size(), hash_sha384);

            // Verify the ECDSA signature
            ECDSA_do_verify(hash_sha384, SHA384_DIGEST_LENGTH, signature, key);
            break;
    }

    // Clean up
    ECDSA_SIG_free(signature);

    return true;
}
