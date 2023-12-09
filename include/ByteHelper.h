#pragma once

#include <vector>
#include <iostream>
#include <memory>


class ByteHelper
{
public:
    /// @brief Converts a sequence of bytes to an integer
    /// @param input The sequence of bytes to convert
    /// @return The result of the conversion
    static int bytesToInt(const std::vector<unsigned char>& input);

    /// @brief Prints the given vector of bytes as hexadecimal
    /// @param byteVec the bytes which wil be printed
    static void printBytes(const std::vector<unsigned char>& byteVec);

    static std::vector<unsigned char> charArrTobytes(const char input[], int size);
};

