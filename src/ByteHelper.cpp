#include "ByteHelper.h"

#include <iostream>
#include <iomanip>

int ByteHelper::bytesToInt(const std::vector<unsigned char>& input)
{
    int result = 0;

    // Each iteration shifts the bits of the result by 8 to the left, and performs a bitwise or
    // to insert the new byte.
    // This is the method of insertion because the input is in the big-endian byte order
    for(unsigned char b : input)
    {
        result = (result >> 8) | ((int)b);
    }

    return result;
}

void ByteHelper::printBytes(const std::vector<unsigned char>& byteVec)
{
    std::cout << "0x";

    for(unsigned char b : byteVec)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }

    // Return back to decimal
    std::cout << std::dec;
}

