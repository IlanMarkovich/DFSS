#include "RegexFilter.h"

const std::map<char,char> RegexFilter::similar_chars_map = {
    
    {'o','O'},
    {'O','0'},
    {'0','o'},

    {'s','z'},
    {'z','$'},
    {'$','5'},
    {'5','S'},
    {'S','Z'},
    {'Z','2'},
    {'2','s'},
    
    {'b','6'},
    {'6','G'},
    {'G','b'},

    {'g','9'},
    {'9','q'},
    {'q','g'},

    {'L','l'},
    {'l','1'},
    {'1','|'},
    {'|','I'},
    {'I','L'},

    {'V','v'},
    {'v','U'},
    {'U','u'},
    {'u','V'},
};

std::string RegexFilter::GetSimillercharacters(char c)
{
    std::string similar_chars = "[";
    char next = c;
    while((next = similar_chars_map.at(next)) != c) 
        {
            similar_chars += next;
            similar_chars += ',';
        }
    similar_chars.pop_back(); // remove last ,
    similar_chars += "]"; 
    return similar_chars;
}

RegexFilter::RegexFilter(std::string url)
{
    if(url.find(".") < MIN_LENGTH)
    {
        throw RegexFilterSizeException(); // the string is too small to make assumption
    }
    this->url = url;


    this->regexPatternSame = "^" + url + "$";
    this->regexPatternOneDiffrence = "";
    this->regexPatternTwoDiffrence = "";
    this->regexPatternLettersAdded = "[" + url + "]*([^" + url + "])[" + url + "]*";

    for (size_t i = 0; i < url.length(); ++i) {
        char curr_first_char = url[i];
        
        if(similar_chars_map.find(curr_first_char) == similar_chars_map.end())// curr character has no simller characters in the map 
        {
            continue;
        }

        regexPatternOneDiffrence += "|^" + url.substr(0, i); // add the start of the url
        std::string similar_chars_for_first = GetSimillercharacters(curr_first_char);

        for (size_t j = i + 1; j < url.length(); ++j)
        {
            char curr_second_char = url[j];
            if(similar_chars_map.find(curr_second_char) == similar_chars_map.end())// curr character has no simller characters in the map 
            {
                continue;
            }
            regexPatternTwoDiffrence += "|^" + url.substr(0, i) + similar_chars_for_first + url.substr(i + 1, j - i - 1);
            std::string similar_chars_for_second = GetSimillercharacters(curr_second_char);

            regexPatternTwoDiffrence += similar_chars_for_second + url.substr(j + 1) + "$";
        }

        regexPatternOneDiffrence += similar_chars_for_first + url.substr(i + 1) + "$"; // add the end of the url and the similler characters
    }
}

bool RegexFilter::Filter(std::string url)
{
    if(url.find(".") <= MIN_LENGTH)
    {
        return false; // the string is too small to make assumption
    }
    int LengthDifference = abs(url.size() - this->url.size());

    if(LengthDifference > 0)
    {
        return FilterDifferentLength(url);
    }
    return FilterSameLength(url);
}


bool RegexFilter::FilterSameLength(std::string url)
{
    if(std::regex_match(url, std::regex(regexPatternSame))) 
    {
        return false; // same url
    }
    if(std::regex_match(url, std::regex(regexPatternTwoDiffrence)) || std::regex_match(url, std::regex(regexPatternOneDiffrence)))
    {
        return true; // one or two characters are wrong is suspicious
    }

    return false; // three or more characters are wrong so its not suspicious
}

bool RegexFilter::FilterDifferentLength(std::string url)
{
    int LengthDifference = abs(url.size() - this->url.size());
    if(LengthDifference > MAX_LENGTH_DIFF) 
    {
        return false;// the difference is too big to make assumption
    }
    
    return equals(url.substr(0, url.find('.')), this->url.substr(0, this->url.find('.')));
}

bool RegexFilter::equals(std::string a, std::string b) 
{
    if(a == b)
        return false;

    // Default number of mistakes allowed
    int mistakes_allowed = (a.size()/4 + b.size()/4) / 2 - std::abs((int)(a.size() - b.size()));

    if(mistakes_allowed <= 0)
        return false;

    for(size_t i = 0; i < std::min(a.size(), b.size()); i++) { // go from first to last character index the words
        if(a[i] != b[i]) { // if this character from word 1 does not equal the character from word 2
            mistakes_allowed--; // reduce one mistake allowed
            if(mistakes_allowed < 0) { // and if you have more mistakes than allowed
                return false; // return false
            }
        }
    }
    return true;
}