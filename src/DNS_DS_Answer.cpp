#include "DNS_DS_Answer.h"

// C'tor
DNS_DS_Answer::DNS_DS_Answer(int type, const std::vector<unsigned char>& dnsMsg, int & index)
: DNS_Answer(type, dnsMsg, index)
{
    _key_id = DNS_Reader::readPortionFromMessage(dnsMsg, index);

    const int ALGORITHM_SIZE = 1;
    _algorithm = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, ALGORITHM_SIZE));

    const int DIGEST_TYPE_SIZE = 1;
    _digest_type = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, DIGEST_TYPE_SIZE));

    int remaining_data_len = _data_len - DNS_PROPERTY_SIZE - ALGORITHM_SIZE - DIGEST_TYPE_SIZE;
    _digest = DNS_Reader::readPortionFromMessage(dnsMsg, index, remaining_data_len);
}

// Getters

std::vector<unsigned char> DNS_DS_Answer::getDigest() const
{
    return _digest;
}
