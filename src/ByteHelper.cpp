#include "ByteHelper.h"

#include <iostream>
#include <iomanip>

int ByteHelper::bytesToInt(const bytes& input)
{
    int result = 0;

    // Each iteration shifts the bits of the result by 8 to the left, and performs a bitwise or
    // to insert the new byte.
    // This is the method of insertion because the input is in the big-endian byte order
    for(byte b : input)
    {
        result = (result >> 8) | ((int)b);
    }

    return result;
}

void ByteHelper::printBytes(const bytes& byteVec)
{
    std::cout << "0x";

    for(byte b : byteVec)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }

    // Return back to decimal
    std::cout << std::dec;
}