/*
 Copyright (c) 2016, Jason Naldi
 
 - direct contact: dev@jasonnaldi.com
 - web: https://jasonnaldi.com
 - github: https://github.com/jasonnaldi
 
 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
 
 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "stringUtils.hpp"

void stringUtils::replaceAllOccurrenciesOfChar(std::string &str, char existingChar, char replacement)
{
    for (auto it = str.begin(); it < str.end(); ++it)
    {
        if (*it == existingChar)
        {
            *it = replacement;
        }
    }
}

void stringUtils::replaceAllOccurrenciesOfPattern(std::string &str, const std::string &pattern, const std::string &newPattern)
{
    ssize_t idx = str.find(pattern);
    do
    {
        idx = str.find(pattern);
        if (idx != std::string::npos)
        {
            //Pattern found
            
            str.replace(idx, idx + pattern.length(), newPattern);
            
            idx += pattern.length();
        }
    }
    while (idx != std::string::npos);
}

void stringUtils::eraseAllOccurrenciesOfChar(std::string &str, char c)
{
    for (auto it = str.begin(); it < str.end(); ++it)
    {
        if (*it == c)
        {
            str.erase(it);
        }
    }
}

void stringUtils::eraseAllOccurrenciesOfPattern(std::string &str, const std::string &pattern)
{
    ssize_t idx = 0;
    do
    {
        idx = str.find(pattern);
        if (idx != std::string::npos)
        {
            str.erase(idx, pattern.length());
        }
    }
    while (idx != std::string::npos);
}

std::vector<std::string> stringUtils::stringToVector(const std::string &str, char separator)
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

std::vector<std::string> stringUtils::stringToVector(const std::string &str, const std::string& separator)
{
    std::vector<std::string> ret;
    
    ssize_t idx = 0;
    
    do
    {
        ssize_t prevIdx = idx;
        
        idx = str.find(separator);
        
        ret.push_back(str.substr(prevIdx, (idx == std::string::npos ? str.length() : idx) - prevIdx));
    }
    while (idx != std::string::npos);
    
    return ret;
}

void stringUtils::trim(std::string &str)
{
    for (auto it = str.begin(); it < str.end(); ++it)
    {
        if (*it == ' ' && it < str.end() - 1 && *(it + 1) == ' ')
        {
            str.erase(it);
        }
    }
}
