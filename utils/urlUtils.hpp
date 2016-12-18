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

#ifndef urlUtils_hpp
#define urlUtils_hpp

#include <string>

#include "htmlUtils.hpp"

namespace urlUtils
{
    /*
     @brief: given a url string, return whether it is valid or not.
     
     @param `url` Any of:
        - anchor inside html
        - link to file on another machine
        - file in local machine
        - anchor inside file in local machine.
     @param `pwd` The path relative to the url. Leave empty if the url is to a file *not* on the local machine.
     @param `html` if available, pass the html tree where to search for anchors.
     
     @return bool.
     */
    bool isUrlValidRelativeToPath(const std::string &url, const std::string &pwd, const elementsTree_t &html);
}

#endif /* urlUtils_hpp */
