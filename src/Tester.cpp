#include "Tester.h"

#include <iostream>

using std::cout;
using std::endl;

// Public Methods

void Tester::dnsSerialization(const bytes& message)
{
    DnsMessage dns_msg(message);

    cout << "Transaction ID: ";
    ByteHelper::printBytes(dns_msg.getTransactionId());
    cout << endl;

    cout << "Flags: ";
    ByteHelper::printBytes(dns_msg.getFlags());
    cout << endl;

    cout << "Questions: " << dns_msg.getQuestions() << endl;
    cout << "Answers RRs: " << dns_msg.getAnswers_RRs() << endl;
    cout << "Authority RRs: " << dns_msg.getAuthority_RRs() << endl;
    cout << "Additional RRs: " << dns_msg.getAdditional_RRs() << endl;

    Query query = dns_msg.getQuery();
    cout << "Query name: " << query.name << endl;
    cout << "Query type: " << query.type << endl;
    cout << "Query class: ";
    ByteHelper::printBytes(query.queryClass);
    cout << endl;
}
