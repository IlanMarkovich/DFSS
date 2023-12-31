#include "DNS_RRSIG_Answer.h"

// C'tor
DNS_RRSIG_Answer::DNS_RRSIG_Answer(const std::string& name, const std::vector<unsigned char>& dnsMsg, int& index)
: DNS_Answer(name, dnsMsg, index)
{
    const int TYPE_COVERED_SIZE = 4;
    _type_covered = ByteHelper::bytesToInt(DnsMessage::readPortionFromMessage(dnsMsg, index, TYPE_COVERED_SIZE));

    const int ALGORITHM_SIZE = 1;
    _algorithm = ByteHelper::bytesToInt(DnsMessage::readPortionFromMessage(dnsMsg, index, ALGORITHM_SIZE));

    const int LABELS_SIZE = 1;
    _labels = ByteHelper::bytesToInt(DnsMessage::readPortionFromMessage(dnsMsg, index, LABELS_SIZE));

    const int ORIGINAL_TTL_SIZE = 4;
    _original_ttl = ByteHelper::bytesToInt(DnsMessage::readPortionFromMessage(dnsMsg, index, ORIGINAL_TTL_SIZE));

    const int SIG_EXPIRATION_SIZE = 4;
    _sig_expiration = DnsMessage::readPortionFromMessage(dnsMsg, index, SIG_EXPIRATION_SIZE).data();

    const int SIG_INCEPTION_SIZE = 4;
    _sig_inception = DnsMessage::readPortionFromMessage(dnsMsg, index, SIG_INCEPTION_SIZE).data();

    _key_tag = ByteHelper::bytesToInt(DnsMessage::readPortionFromMessage(dnsMsg, index));
    _signer_name = DnsMessage::readStringFromMessage(dnsMsg, index);

    int remaining_data_len = _data_len - TYPE_COVERED_SIZE - ALGORITHM_SIZE - LABELS_SIZE - ORIGINAL_TTL_SIZE - SIG_EXPIRATION_SIZE - SIG_INCEPTION_SIZE - (2 * DNS_PROPERTY_SIZE);
    _signature = DnsMessage::readPortionFromMessage(dnsMsg, index, remaining_data_len);
}
