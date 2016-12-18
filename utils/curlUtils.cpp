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
#include "shellUtils.hpp"

std::string curlUtils::getWebsiteState(const std::string &url)
{
    return shellUtils::exec("curl -IsSk \"" + url + "\" | head -1");
}

bool curlUtils::isWebsiteOk(const std::string &url)
{
    // BUGFIX:
    // -k disables insecure certificates. After all, this only checks the health of a website
    // No sensitive data going through...
    
    std::string response = curlUtils::getWebsiteState(url);
    
    return
    response.find("40") == std::string::npos &&
    response.find("41") == std::string::npos &&
    response.find("50") == std::string::npos &&
    response.find("Operation timed out") == std::string::npos;
}

std::string curlUtils::validateHTML(const std::string &path)
{
    // Validate via validator.w3.org
    /*
     Source: https://github.com/validator/validator/wiki/Service:-Input:-POST-body
     */
    std::string curlValidateCommand =
    "curl -sS -H \"Content-Type: text/html; charset=utf-8\" "
    "--data-binary \"@" + path + "\" "
    "https://validator.w3.org/nu/?out=json";
    
    return shellUtils::exec(curlValidateCommand);
}
