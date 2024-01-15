#include "DNSSEC.h"

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

        delete domain_DNSKEY_response;
        delete TLD_DS_response;
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

        if(!verifyData(ksk, ksk, sig))
            return false;
    }

    if(data_response == nullptr)
        return true;

    DNS_Answer data = data_response->getResponse_RRset<DNS_Answer>()[0];
    DNS_RRSIG_Answer data_sig = data_response->getResponse_RRset<DNS_RRSIG_Answer>()[0];

    for(const auto& key : keys)
    {
        if(!verifyData(data, key, data_sig))
            return false;
    }

    return true;
}

bool DNSSEC::verifyData(const DNS_Answer & data, const DNS_DNSKEY_Answer & key, const DNS_RRSIG_Answer & signature) const
{
    int algorithm = key.getAlgorithm();
    const EVP_MD* hash = algorithmToHash.at(algorithm);
    DNSSEC_Encryption enc = algorithmToEncrpytion.at(algorithm);

    if(enc == DNSSEC_RSA)
    {
        RSA* rsaKey = publicKeyRSA(key.getPublicKey());

        if(rsaKey == nullptr)
            return false;
        
        return verifyPKCS1v15(rsaKey, data.getData(), signature.getSignature(), hash);
    }
    else if(enc == DNSSEC_ECDSA)
    {
        EC_KEY* ecKey = publicKeyECDSA(key.getPublicKey(), algorithm);
        auto sigbuf = signature.getSignature();

            if (sigbuf.size() % 2 != 0) {
            // Ensure that the sigbuf has an even length
            return false;
        }

        size_t halfSize = sigbuf.size() / 2;

        BIGNUM* r = BN_bin2bn(sigbuf.data(), halfSize, nullptr);
        BIGNUM* s = BN_bin2bn(sigbuf.data() + halfSize, halfSize, nullptr);

        if (!r || !s) {
            BN_free(r);
            BN_free(s);
            return false;
        }

        return verifyECDSA(ecKey, data.getData(), r, s, hash);
    }

    return false;
}

RSA* DNSSEC::publicKeyRSA(const std::vector<unsigned char>& dnsKey) const
{
    if (dnsKey.size() < 1 + 1 + 64) {
        // Exponent must be at least 1 byte and modulus at least 64
        return nullptr;
    }

    uint16_t explen = dnsKey[0];
    int keyoff = 1;
    if (explen == 0) {
        explen = (dnsKey[1] << 8) | dnsKey[2];
        keyoff = 3;
    }

    if (explen > 4 || explen == 0 || dnsKey[keyoff] == 0) {
        // Exponent larger than supported by the crypto package,
        // empty, or contains prohibited leading zero.
        return nullptr;
    }

    int modoff = keyoff + explen;
    int modlen = dnsKey.size() - modoff;
    if (modlen < 64 || modlen > 512 || dnsKey[modoff] == 0) {
        // Modulus is too small, large, or contains prohibited leading zero.
        return nullptr;
    }

    RSA* pubkey = RSA_new();
    if (!pubkey) {
        return nullptr;
    }

    // Set the exponent
    BIGNUM* e = BN_bin2bn(&dnsKey[keyoff], explen, nullptr);
    if (!e || RSA_set0_key(pubkey, nullptr, nullptr, e) != 1) {
        RSA_free(pubkey);
        BN_free(e);
        return nullptr;
    }

    // Set the modulus
    BIGNUM* n = BN_bin2bn(&dnsKey[modoff], modlen, nullptr);
    if (!n || RSA_set0_key(pubkey, nullptr, n, nullptr) != 1) {
        RSA_free(pubkey);
        BN_free(e);
        BN_free(n);
        return nullptr;
    }

    return pubkey;
}

EC_KEY * DNSSEC::publicKeyECDSA(const std::vector<unsigned char>& dnsKey, int algorithm) const
{
    EC_KEY* pubkey = EC_KEY_new_by_curve_name(NID_undef);
    if (!pubkey) {
        return nullptr;
    }

    switch (algorithm) {
        case 13:
            EC_KEY_set_group(pubkey, EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1));
            if (dnsKey.size() != 64) {
                // wrongly encoded key
                EC_KEY_free(pubkey);
                return nullptr;
            }
            break;
        case 14:
            EC_KEY_set_group(pubkey, EC_GROUP_new_by_curve_name(NID_secp384r1));
            if (dnsKey.size() != 96) {
                // wrongly encoded key
                EC_KEY_free(pubkey);
                return nullptr;
            }
            break;
        default:
            EC_KEY_free(pubkey);
            return nullptr;
    }

    const std::vector<uint8_t> x_keybuf(dnsKey.begin(), dnsKey.begin() + dnsKey.size() / 2);
    const std::vector<uint8_t> y_keybuf(dnsKey.begin() + dnsKey.size() / 2, dnsKey.end());

    BIGNUM* x = BN_bin2bn(x_keybuf.data(), x_keybuf.size(), nullptr);
    BIGNUM* y = BN_bin2bn(y_keybuf.data(), y_keybuf.size(), nullptr);

    if (!x || !y || EC_KEY_set_public_key_affine_coordinates(pubkey, x, y) != 1) {
        BN_free(x);
        BN_free(y);
        EC_KEY_free(pubkey);
        return nullptr;
    }

    BN_free(x);
    BN_free(y);

    return pubkey;
}

bool DNSSEC::verifyPKCS1v15(RSA * key, const std::vector<unsigned char>& encryptedData, const std::vector<unsigned char>& signature, const EVP_MD * hashFunction) const
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

    int result = EVP_DigestVerifyFinal(mdCtx, signature.data(), signature.size());

    EVP_MD_CTX_free(mdCtx);
    EVP_PKEY_free(evpKey);

    return result == 1;
}

bool DNSSEC::verifyECDSA(EC_KEY * key, const std::vector<unsigned char>& data, const BIGNUM * r, const BIGNUM * s, const EVP_MD * hashFunction) const
{
    if (!key || data.empty() || !r || !s || !hashFunction) {
        return false;
    }

    ECDSA_SIG* signature = ECDSA_SIG_new();
    if (!signature) {
        return false;
    }

    if (ECDSA_SIG_set0(signature, const_cast<BIGNUM*>(r), const_cast<BIGNUM*>(s)) != 1) {
        ECDSA_SIG_free(signature);
        return false;
    }

    EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
    if (!mdCtx) {
        ECDSA_SIG_free(signature);
        return false;
    }

    if (EVP_DigestVerifyInit(mdCtx, nullptr, hashFunction, nullptr, nullptr) != 1) {
        EVP_MD_CTX_free(mdCtx);
        ECDSA_SIG_free(signature);
        return false;
    }

    if (EVP_DigestVerifyUpdate(mdCtx, data.data(), data.size()) != 1) {
        EVP_MD_CTX_free(mdCtx);
        ECDSA_SIG_free(signature);
        return false;
    }

    int result = ECDSA_do_verify(data.data(), static_cast<int>(data.size()), signature, key);

    EVP_MD_CTX_free(mdCtx);
    ECDSA_SIG_free(signature);

    return result == 1;
}
