#pragma once

#include <iostream>
#include <string>

#include "DnsMessage.h"

using std::cout;
using std::endl;
using std::string;


class Tester
{
public:
    /// @brief Tests the serialization of a dns message by printing it to the console
    /// @param message The DNS message which is being serialized
    static void dnsSerialization(const bytes& message);

private:
    static void printBytes(const bytes& byteVec);
};