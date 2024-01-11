#pragma once

#include <string>

#include "ByteHelper.h"
#include "DNS_Reader.h"
#include "DNS_A_Answer.h"
#include "DNS_RRSIG_Answer.h"
#include "DNS_DNSKEY_Answer.h"
#include "DNS_DS_Answer.h"

#define DNS_PROPERTY_SIZE 2
#define DNS_A 1
#define DNS_RRSIG 46
#define DNS_DNSKEY 48
#define DNS_DS 43

using std::string;


struct Query
{
    string name;
    int type;
    const unsigned char* queryClass;
};

class DnsMessage
{
private:
    // Fields

    std::vector<unsigned char> _messageInBytes;
    std::vector<unsigned char> _transactionId;
    std::vector<unsigned char> _flags;
    int _queries;
    int _answers_RRs;
    int _authority_RRs;
    int _additional_RRs;
    Query _query;
    std::vector<DNS_Answer*> _answers;
    bool _DNSSEC_response;

public:
    // C'tor
    DnsMessage(const std::vector<unsigned char>& message);

    // D'tor
    ~DnsMessage();

    // Getters

    Query getQuery() const;
    std::vector<unsigned char> getMessageInBytes() const;
    bool is_DNSSEC_response() const;

    // Methods

    /// @brief Gets a certain DNS RRset from a response
    /// @tparam T The type of RRset requested
    /// @return The RRset from the response (could be empty)
    template <class T>
    std::vector<T> getResponse_RRset() const;

    /// @brief Adds the additional RR called OPT with the DO flag to
    /// tell the DNS resolver to use DNSSEC
    void addOPT();

    /// @brief Change the query name to only the TLD name
    void changeToTLD();

    /// @brief Change the query name to a DNS root server
    void changeToRoot();

    /// @brief Changes the type of the query to a certain type
    /// @param type The type of the new DNS query type
    void changeMessageQueryType(int type);
};

// Public Template Methods

template<class T>
inline std::vector<T> DnsMessage::getResponse_RRset() const
{
    std::vector<T> RRset;

    for(auto answer : _answers)
    {
        T* ans;

        if((ans = dynamic_cast<T*>(answer)) != nullptr)
            RRset.push_back(*ans);
    }

    return RRset;
}
