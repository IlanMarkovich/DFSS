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
};
