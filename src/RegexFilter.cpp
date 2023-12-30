#include "RegexFilter.h"

RegexFilter::RegexFilter(std::string url)
{
    if (url.size() < MIN_LENGTH)
    {
        throw std::exception(); // the string is too small to make assumption
    }
    this->url = url;


    this->regexPatternSame = "^" + url + "$";
    this->regexPatternOneDiffrence = "";
    this->regexPatternTwoDiffrence = "";
    this->regexPatternLettersAdded = "^";

    for (size_t i = 0; i < url.length(); ++i) {
        regexPatternOneDiffrence += "|^" + url.substr(0, i) + "." + url.substr(i + 1) + "$";
        regexPatternLettersAdded += url[i];
        regexPatternLettersAdded += ".*";

        for (size_t j = i + 1; j < url.length(); ++j)
        {
            regexPatternTwoDiffrence += "|^" + url.substr(0, i) + "." + url.substr(i + 1, j - i - 1) + "." + url.substr(j + 1) + "$";
        }
    }
    regexPatternLettersAdded += "";
}

bool RegexFilter::Filter(std::string url)
{
    if (url.size() < MIN_LENGTH)
    {
        throw std::exception(); // the string is too small to make assumption
    }
    int LengthDifference = abs(int(url.size() - this->url.size()));

    if (LengthDifference > 0)
    {
        return FilterDifferentLength(url);
    }
    return FilterSameLength(url);
}


bool RegexFilter::FilterSameLength(std::string url)
{
    if (std::regex_match(url, std::regex(regexPatternSame)))
    {
        return false; // same url
    }
    if (std::regex_match(url, std::regex(regexPatternTwoDiffrence)) || std::regex_match(url, std::regex(regexPatternOneDiffrence)))
    {
        return true; // one or two characters are wrong is suspicious
    }

    return false; // three or more characters are wrong so its not suspicious
}

bool RegexFilter::FilterDifferentLength(std::string url)
{
    int LengthDifference = url.size() - this->url.size();
    if (LengthDifference > MAX_LENGTH_DIFF)
    {
        return false;// the difference is too big to make assumption
    }
    else if (LengthDifference > 0)
    {
        return std::regex_search(url, std::regex(regexPatternLettersAdded)); // the same url but random charcters have been removed 
    }

    std::string regexPatternLettersAddedGivenUrl = "^";
    for (size_t i = 0; i < url.length(); ++i)
    {
        regexPatternLettersAddedGivenUrl += url[i];
        regexPatternLettersAddedGivenUrl += ".*";
    }
    return std::regex_search(this->url, std::regex(regexPatternLettersAddedGivenUrl)); // the same url but random charcters have been added 
}