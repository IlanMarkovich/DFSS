#include <iostream>
#include <map>
#include <regex>
#include "RegexFilterSizeException.h"

#define MIN_LENGTH 3
#define MAX_LENGTH_DIFF 2

class RegexFilter
{
    private:
        std::string url;
        std::string regexPatternSame;
        std::string regexPatternOneDiffrence;
        std::string regexPatternTwoDiffrence;
        std::string regexPatternLettersAdded;
        bool FilterSameLength(std::string url);
        bool FilterDifferentLength(std::string url);
        std::string GetSimillercharacters(char c);
        static const std::map<char, char> similar_chars_map;
    public:
        RegexFilter(std::string url);
        bool Filter(std::string url);
};
