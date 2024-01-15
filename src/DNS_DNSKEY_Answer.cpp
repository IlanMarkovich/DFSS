#include "DNS_DNSKEY_Answer.h"

// C'tor
DNS_DNSKEY_Answer::DNS_DNSKEY_Answer(int type, const std::vector<unsigned char>& dnsMsg, int & index)
: DNS_Answer(type, dnsMsg, index)
{
    _flags = DNS_Reader::readPortionFromMessage(dnsMsg, index);

    const int PROTOCOL_SIZE = 1;
    _protocol = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, PROTOCOL_SIZE));

    const int ALGORITHM_SIZE = 1;
    _algorithm = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, ALGORITHM_SIZE));

    int remaining_data_len = _data_len - DNS_PROPERTY_SIZE - PROTOCOL_SIZE - ALGORITHM_SIZE;
    _public_key = DNS_Reader::readPortionFromMessage(dnsMsg, index, remaining_data_len);
}

// Getters

int DNS_DNSKEY_Answer::getAlgorithm() const
{
    return _algorithm;
}

std::vector<unsigned char> DNS_DNSKEY_Answer::getPublicKey() const
{
    return _public_key;
}

// Public Methods

DNS_DNSKEY_Answer DNS_DNSKEY_Answer::extractKSK(const std::vector<DNS_DNSKEY_Answer>& keys)
{
    if(keys.size() == 1)
        return keys[0];

    for(const auto& key : keys)
    {
        // Checks if the current key is a KSK by checking the Key Signing Key flag
        if(key._flags[1] == '\1')
            return key;
    }

    // There should be a KSK in the given vector
    throw std::exception();
}
