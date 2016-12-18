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

#include "curlUtils.hpp"
#include "fileUtils.hpp"
#include "shellUtils.hpp"
#include "stringUtils.hpp"
#include "urlUtils.hpp"

#include <fstream>
#include <iostream>
#include <thread>

bool urlUtils::isUrlValidRelativeToPath(const std::string &url, const std::string &pwd, const elementsTree_t &html)
{
    bool available = false;
    
    // Foud kinds of url:
    // Website
    if (url.substr(0, 4).compare("http") == 0 || url.substr(0, 3).compare("www") == 0)
    {
        available = curlUtils::isWebsiteOk(url);
    }
    // Internal anchor
    else if (url.front() == '#')
    {
        auto foundTagsWithLinkId = htmlUtils::extractElementsMatchingPatternFromTree(html, "", {{"id", url.substr(1, url.length() - 1)}});
        
        available = foundTagsWithLinkId.size() > 0;
    }
    // For pages on local machine, remember that the doc might be on a different dir than this executable.
    // External page on local machine, no anchor
    else if (url.find("#") == std::string::npos)
    {
        std::string pathRelativeToExecutable = pwd + "/" + url;
        pathRelativeToExecutable = stringUtils::replaceAllOccurrencies(pathRelativeToExecutable, "%20", " ");
        available = fileUtils::doesFileExist(pathRelativeToExecutable);
    }
    else
    {
        ssize_t hashIdx = url.find("#");
        // No need to check for npos hashIdx, since it has already been done in the previous case
        
        std::string linkPath = url.substr(0, hashIdx);
        linkPath = pwd + "/" + linkPath;
        
        linkPath = stringUtils::replaceAllOccurrencies(linkPath, "%20", " ");
        
        std::string itemId = url.substr(hashIdx + 1, url.length() - hashIdx - 1);
        
        available = fileUtils::doesFileExist(linkPath);
        
        // Do not look for anchor if file does not exist
        if (available)
        {
            std::ifstream ifs(linkPath);
            std::string str((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
            
            available = str.find(itemId) != std::string::npos;
        }
    }
    
    return available;
}
