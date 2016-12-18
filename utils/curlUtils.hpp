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

#ifndef curlUtils_hpp
#define curlUtils_hpp

#include <string>

namespace curlUtils
{
    
    /**
     <#Description#>

     @param url <#url description#>
     @return <#return value description#>
     */
    std::string getWebsiteState(const std::string &url);
    
    /*
     @brief: given an url to a website, return whether the website's answer was 200.
     
     #TODO
     Known bugs:
      - unable to catch timeout errors
     
     @param `url` A string representation of the url.
     
     @return bool.
     */
    bool isWebsiteOk(const std::string &url);
    
    /*
     @brief: given a path to a file, send that file to w3's validator api and return
            its response as a string.
     
     @param `path` The path to a file.
     
     @return std::string.
     */
    std::string validateHTML(const std::string &path);
}

#endif /* curlUtils_hpp */
