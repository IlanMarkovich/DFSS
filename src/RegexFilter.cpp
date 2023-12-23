#include "RegexFilter.h"

RegexFilter::RegexFilter(std::string url)
{
    if(url.size() < MIN_LENGTH)
    {
        throw RegexFilterSizeException(); // the string is too small to make assumption
    }
    this->url = url;


    this->regexPatternSame = "^" + url + "$";
    this->regexPatternOneDiffrence = "";
    this->regexPatternTwoDiffrence = "";
    this->regexPatternLettersAdded = "[" + url + "]*([^" + url + "])[" + url + "]*";

    for (size_t i = 0; i < url.length(); ++i) {
        regexPatternOneDiffrence += "|^" + url.substr(0, i) + "." + url.substr(i + 1) + "$";
        for (size_t j = i + 1; j < url.length(); ++j)
        {
            regexPatternTwoDiffrence += "|^" + url.substr(0, i) + "." + url.substr(i + 1, j - i - 1) + "." + url.substr(j + 1) + "$";
        }
    }
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
    else if(std::regex_search(url, std::regex(regexPatternLettersAdded)))
    {
        return true; // the same url but random charcters have been added 
    }
    return false;
}