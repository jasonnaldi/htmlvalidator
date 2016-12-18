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

#ifndef stringUtils_hpp
#define stringUtils_hpp

#include <string>
#include <vector>

namespace stringUtils
{
    /*
     @brief Given a string `str` and a character `separator`, return an array of substrings of `str`,
            split by `separator`.
     
     @param `str` A string to tokenize.
     @param `separator` The character to use as a separator.
     
     @return std::vector<std::string>.
     */
    std::vector<std::string> tokenize(const std::string &str, char separator);
    
    /*
     @brief Given a string, return the same string but without leading and trailing spaces.
            Also replaces two or more consecutive white spaces with a single white space.
     
     @param `str` A string to trim.
     
     @return void.
     */
    void trim(std::string &str);
    
    /*
     @brief Return the line in a string where a pattern first occurs.
     
     @param `str` A string to scan
     @param `pattern` A pattern to seek
     
     @return ssize_t. Will be std::string::npos if the pattern was not found.
     */
    ssize_t firstLineOccurrence(const std::string &str, const std::string &pattern);
    
    /*
     @brief Convert a string to lowercase
     
     @param `str` A string to convert to lowercase
     
     @return std::string
     */
    std::string lowercase(const std::string &str);
    
    /**
     Replace all occurrencies of a string with another string inside a string.

     @param source The string where to replace occurrencies
     @param oldStr The string to replace
     @param newStr The string to replace the old string with
     @return A string where all occurrencies of oldStr have been replaced by newStr
     */
    std::string replaceAllOccurrencies(const std::string &source, const std::string &oldStr, const std::string &newStr);
}

#endif /* stringUtils_hpp */
