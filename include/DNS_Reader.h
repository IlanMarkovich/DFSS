#pragma once

#include <string>
#include <vector>

#define DNS_PROPERTY_SIZE 2


class DNS_Reader
{
public:
    // Methods

    /// @brief Reads a portion of a bytes sequence (reffered as message) from a certain index
    /// @param message The sequence of bytes being read from
    /// @param i The index which the function starts to read from
    /// @param prop_size The property size to calculate the destination index (equals to DNS_PROPERTY_SIZE on default)
    /// @return The new sequence read from the message
    static std::vector<unsigned char> readPortionFromMessage(const std::vector<unsigned char>& message, int& i, int prop_size = DNS_PROPERTY_SIZE);

    /// @brief Read a certain string from a bytes sequence which represents a DNS message
    /// @param message The DNS message in bytes
    /// @param i The start index of the string
    /// @return The string read from the message
    static std::string readStringFromMessage(const std::vector<unsigned char>& message, int& i);
};
