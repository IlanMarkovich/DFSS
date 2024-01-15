#include "DNS_RRSIG_Answer.h"

// C'tor
DNS_RRSIG_Answer::DNS_RRSIG_Answer(int type, const std::vector<unsigned char>& dnsMsg, int& index)
: DNS_Answer(type, dnsMsg, index)
{
    _type_covered = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index));

    const int ALGORITHM_SIZE = 1;
    _algorithm = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, ALGORITHM_SIZE));

    const int LABELS_SIZE = 1;
    _labels = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, LABELS_SIZE));

    const int ORIGINAL_TTL_SIZE = 4;
    _original_ttl = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index, ORIGINAL_TTL_SIZE), true);

    const int SIG_EXPIRATION_SIZE = 4;
    _sig_expiration = DNS_Reader::readPortionFromMessage(dnsMsg, index, SIG_EXPIRATION_SIZE).data();

    const int SIG_INCEPTION_SIZE = 4;
    _sig_inception = DNS_Reader::readPortionFromMessage(dnsMsg, index, SIG_INCEPTION_SIZE).data();

    _key_tag = ByteHelper::bytesToInt(DNS_Reader::readPortionFromMessage(dnsMsg, index), true);
    _signer_name = DNS_Reader::readStringFromMessage(dnsMsg, index);

    int remaining_data_len = _data_len - ALGORITHM_SIZE - LABELS_SIZE - ORIGINAL_TTL_SIZE - SIG_EXPIRATION_SIZE - SIG_INCEPTION_SIZE - (3 * DNS_PROPERTY_SIZE);
    _signature = DNS_Reader::readPortionFromMessage(dnsMsg, index, remaining_data_len);
}

// Getters

int DNS_RRSIG_Answer::getTypeCovered() const
{
    return _type_covered;
}

int DNS_RRSIG_Answer::getAlgorithm() const
{
    return _algorithm;
}

int DNS_RRSIG_Answer::getKeyTag() const
{
    return _key_tag;
}

std::vector<unsigned char> DNS_RRSIG_Answer::getSignature() const
{
    return _signature;
}
