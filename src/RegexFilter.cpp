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

    {'a', '@'},
    {'@', 'a'}
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
    if(url.size() < MIN_LENGTH)
    {
        throw RegexFilterSizeException(); // the string is too small to make assumption
    }
    this->url = url;


    // regex to check if the url is the same
    regexPatternSame = "^" + url + "$";
    // regex to check if you have one char diffrent from the url
    regexPatternOneDiffrence = "";
    // regex to check if you have one char diffrent from the url
    regexPatternTwoDiffrence = "";
    // regex to check if charcters have been added to the same url 
    regexPatternLengthNegtiveOne = ".*";
    // regex to check if charcters have been removed to the same url 
    regexPatternLengthPlasOne = "";

    for (size_t i = 0; i < url.length(); i++)
    {
        char curr_first_char = url[i];
        bool flag_curr_first_Has_similler = similar_chars_map.find(curr_first_char) != similar_chars_map.end();
        
        regexPatternLengthNegtiveOne += url[i];
        regexPatternLengthNegtiveOne += ".*";

        std::string similar_chars_for_first = "";

        if (flag_curr_first_Has_similler)
        {
            regexPatternOneDiffrence += "|^" + url.substr(0, i); // add the start of the url
            similar_chars_for_first = GetSimillercharacters(curr_first_char);
        }
        regexPatternLengthPlasOne += "|.*";
        for (size_t j = 0; j < url.length(); j++)
        {
            char curr_second_char = url[j];
            bool flag_curr_second_Has_similler = similar_chars_map.find(curr_second_char) != similar_chars_map.end();

            if (j != i)
            {
                regexPatternLengthPlasOne += curr_second_char;
                regexPatternLengthPlasOne += ".*";
            }
            else
            {
                if (flag_curr_second_Has_similler)
                {
                    std::string similar_chars_for_second = GetSimillercharacters(curr_second_char);
                    regexPatternLengthPlasOne += similar_chars_for_second;
                }
                else
                {
                    regexPatternLengthPlasOne += curr_second_char;
                }
                regexPatternLengthPlasOne += ".*";
            }

            if (j >= i + 1 && flag_curr_second_Has_similler && flag_curr_first_Has_similler)
            {
                char curr_second_char = url[j];
                regexPatternTwoDiffrence += "|^" + url.substr(0, i) + similar_chars_for_first + url.substr(i + 1, j - i - 1);
                std::string similar_chars_for_second = GetSimillercharacters(curr_second_char);

                regexPatternTwoDiffrence += similar_chars_for_second + url.substr(j + 1) + "$";
            }
        }
        if (flag_curr_first_Has_similler)
        {
            regexPatternOneDiffrence += similar_chars_for_first + url.substr(i + 1) + "$"; // add the end of the url and the similler characters
        }
    }

    regexPatternOneDiffrence = regexPatternOneDiffrence.size() == 0 ? regexPatternOneDiffrence : regexPatternOneDiffrence.substr(1, regexPatternLengthPlasOne.size() - 1);
    regexPatternTwoDiffrence = regexPatternTwoDiffrence.size() == 0 ? regexPatternTwoDiffrence : regexPatternTwoDiffrence.substr(1, regexPatternLengthPlasOne.size() - 1);
    regexPatternLengthPlasOne = regexPatternLengthPlasOne.size() == 0 ? regexPatternLengthPlasOne : regexPatternLengthPlasOne.substr(1, regexPatternLengthPlasOne.size() - 1);
}

bool RegexFilter::Filter(std::string url)
{
    if(url.size() < MIN_LENGTH)
    {
        throw RegexFilterSizeException(); // the string is too small to make assumption
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
    if(LengthDifference == 1)
    {
        return std::regex_match(url, std::regex(regexPatternLengthPlasOne));
    }
    return std::regex_match(url, std::regex(regexPatternLengthNegtiveOne));
}
