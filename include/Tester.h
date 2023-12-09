#pragma once

#include "DnsMessage.h"


class Tester
{
public:
    /// @brief Tests the serialization of a dns message by printing it to the console
    /// @param message The DNS message which is being serialized
    static void dnsSerialization(const bytes& message);
};