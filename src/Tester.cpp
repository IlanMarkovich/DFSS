#include "Tester.h"

// Public Methods

void Tester::dnsSerialization(const bytes& message)
{
    DnsMessage dns_msg(message);

    cout << "Transaction ID: ";
    printBytes(dns_msg.getTransactionId());

    cout << "Flags: ";
    printBytes(dns_msg.getFlags());

    cout << "Questions: " << dns_msg.getQuestions() << endl;
    cout << "Answers RRs: " << dns_msg.getAnswers_RRs() << endl;
    cout << "Authority RRs: " << dns_msg.getAuthority_RRs() << endl;
    cout << "Additional RRs: " << dns_msg.getAdditional_RRs() << endl;

    Query query = dns_msg.getQuery();
    cout << "Query name: " << query.name << endl;
    cout << "Query type: " << query.type << endl;
    cout << "Query class: ";
    printBytes(query.queryClass);
}

// Private Methods

void Tester::printBytes(const bytes& byteVec)
{
    cout << "0x";

    for(byte b : byteVec)
    {
        cout << std::hex << (int)b;
    }

    cout << std::dec << endl;
}
