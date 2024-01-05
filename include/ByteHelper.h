#pragma once

#include <vector>
#include <iostream>
#include <memory>


class ByteHelper
{
public:
    /// @brief Converts a sequence of bytes to an integer
    /// @param input The sequence of bytes to convert
    /// @return The result of the convertion
    static int bytesToInt(const std::vector<unsigned char>& input);

    /// @brief Prints the given vector of bytes as hexadecimal
    /// @param byteVec the bytes which wil be printed
    static void printBytes(const std::vector<unsigned char>& byteVec);

    /// @brief Converts an integer toa sequence of bytes
    /// @param input The integer to convert
    /// @return The result of the convertion
    static std::vector<unsigned char> intToBytes(int input);
};

