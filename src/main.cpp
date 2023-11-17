#include <iostream>

#include "Tester.h"

// If this run is NOT a test
#ifndef TEST
int main()
{
    return 0;
}
#else
int main()
{
    // The example DNS message
     bytes message = {
        static_cast<byte>(0xdf),
        static_cast<byte>(0xbb),
        static_cast<byte>(0x01),
        static_cast<byte>(0x00),
        static_cast<byte>(0x00),
        static_cast<byte>(0x01),
        static_cast<byte>(0x00),
        static_cast<byte>(0x00),
        static_cast<byte>(0x00),
        static_cast<byte>(0x00),
        static_cast<byte>(0x00),
        static_cast<byte>(0x00),
        static_cast<byte>(0x0d),
        'n', 'c', '-', 'u', 'n', 'i', 't', '8', '-', 'm', 'q', 't',
        static_cast<byte>(0x07),
        'n', 'o', 'r', 'd', 'v', 'p', 'n',
        static_cast<byte>(0x03),
        'c', 'o', 'm',
        static_cast<byte>(0x00),
        static_cast<byte>(0x00),
        static_cast<byte>(0x01),
        static_cast<byte>(0x00),
        static_cast<byte>(0x01)
    };

    Tester::dnsSerialization(message);

    return 0;
}
#endif