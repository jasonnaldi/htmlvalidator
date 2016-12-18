/*
 MIT License
 
 Copyright (c) 2016 Jason Naldi
 
 - direct contact: dev@jasonnaldi.com
 - web: https://jasonnaldi.com
 - github: https://github.com/jasonnaldi
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "stringUtils.hpp"

std::vector<std::string> stringUtils::tokenize(const std::string &str, char separator)
{
    std::vector<std::string> ret{""};
    
    if (str.length() > 0)
    {
        // The -1 is to ignore the new line after the last ret
        for (ssize_t i = 0; i < str.size(); ++i)
        {
            if (str.at(i) == separator)
            {
                ret.push_back("");
            }
            else
            {
                ret.back() += str.at(i);
            }
        }
    }
    
    return ret;
}

void stringUtils::trim(std::string &str)
{
    while (str.length() > 0 && str.front() == ' ') {
        str.erase(str.begin());
    }
    
    for (auto it = str.begin(); it < str.end(); ++it)
    {
        if (*it == ' ' && it < str.end() - 1 && *(it + 1) == ' ')
        {
            str.erase(it);
            
            // Bug fix: to make sure the case where the next ' ' is not skipped.
            --it;
        }
    }
    
    while (str.length() > 0 && str.back() == ' ')
    {
        str.pop_back();
    }
}

ssize_t stringUtils::firstLineOccurrence(const std::string &str, const std::string &pattern)
{
    auto idx = str.find(pattern);
    
    if (idx == std::string::npos)
    {
        return std::string::npos;
    }
    
    ssize_t linesCount = 1;
    
    for (auto it = str.begin(); it < str.begin() + idx; ++it)
    {
        if (*it == '\n')
        {
            ++linesCount;
        }
    }
    
    return linesCount;
}

std::string stringUtils::lowercase(const std::string &str)
{
    std::string ret = str;
    
    std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    
    return ret;
}

std::string stringUtils::replaceAllOccurrencies(const std::string &source, const std::string &oldStr, const std::string &newStr)
{
    std::string ret = source;
    
    while (ret.find(oldStr) != std::string::npos) {
        ret.replace(ret.find(oldStr), oldStr.length(), newStr);
    }
    
    return ret;
}

