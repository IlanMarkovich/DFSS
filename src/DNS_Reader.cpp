#include "DNS_Reader.h"

// Public Methods

std::vector<unsigned char> DNS_Reader::readPortionFromMessage(const std::vector<unsigned char>& message, int& i, int prop_size)
{
    int destIndex = i + prop_size;
    std::vector<unsigned char> out;

    for(; i < destIndex; i++)
    {
        out.push_back(message[i]);
    }

    return out;
}

std::string DNS_Reader::readStringFromMessage(const std::vector<unsigned char>& message, int & i)
{
    std::string str;

    for(i++ ; i < (int)message.size() && message[i] != '\0'; i++)
    {
        // If the char is not a displayable character, insert a dot
        str += message[i] <= '\31' ? '.' : message[i];
    }

    i++;
    return str;
}
