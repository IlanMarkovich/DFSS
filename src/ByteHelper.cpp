#include "ByteHelper.h"

#include <iostream>
#include <iomanip>

int ByteHelper::bytesToInt(const std::vector<unsigned char>& input, bool littleEndian)
{
    int result = 0;

    // Each iteration shifts the bits of the result by 8 to the left, and performs a bitwise or
    // to insert the new byte.
    for(unsigned char b : input)
    {
        result = (littleEndian ? (result << 8) : (result >> 8)) | ((int)b);
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

std::vector<unsigned char> ByteHelper::intToBytes(int input)
{
    std::vector<unsigned char> result(2);

    // Extract the two bytes from the integer
    result[0] = static_cast<unsigned char>((input >> 8) & 0xFF); // Most significant byte
    result[1] = static_cast<unsigned char>(input & 0xFF);        // Least significant byte

    return result;
}
