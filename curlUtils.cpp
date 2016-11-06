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

#include "curlUtils.hpp"

#include "shellUtils.hpp"

bool curlUtils::doesWebsite404(const std::string &url)
{
    std::string command = "curl -IsS \"" + url + "\" | head -1";

    std::string response = shellUtils::exec(command);
    
    return response.find("404") != std::string::npos;
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
