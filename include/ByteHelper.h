#pragma once

#include <vector>

#define byte unsigned char
#define bytes std::vector<byte>


class ByteHelper
{
public:
    /// @brief Converts a sequence of bytes to an integer
    /// @param input The sequence of bytes to convert
    /// @return The result of the conversion
    static int bytesToInt(const bytes& input);

    /// @brief Prints the given vector of bytes as hexadecimal
    /// @param byteVec the bytes which wil be printed
    static void printBytes(const bytes& byteVec);

    static bytes charArrTobytes(const char input[], int size);
};

