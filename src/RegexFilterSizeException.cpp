#include "RegexFilterSizeException.h"

const char* RegexFilterSizeException::what()
{
    return "Error: the url length is too small to make assumption based on regex";
}