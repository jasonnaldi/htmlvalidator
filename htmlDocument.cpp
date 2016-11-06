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

#include "htmlDocument.hpp"

#include <iostream>

#include "shellUtils.hpp"
#include "stringUtils.hpp"

htmlDocument::htmlDocument(const std::string &text)
: _text(text)
, _tree()
{
    //Ensure white-spaces normalization
    stringUtils::trim(_text);
    
    // Erase comments. They just cause trouble to the parsing functions...
    ssize_t commentIdx = 0;
    while (true)
    {
        commentIdx = _text.find("<!--", commentIdx);
        
        if (commentIdx == std::string::npos)
        {
            // no more comments. Exit
            break;
        }
        
        _text.erase(_text.begin() + commentIdx, _text.begin() + _text.find("-->", commentIdx));
    }
    
    //Erase useless new lines and tabs.
    // Somehow this causes a bug
    //_text.erase(std::remove(_text.begin(), _text.end(), '\n'), _text.end());
    //_text.erase(std::remove(_text.begin(), _text.end(), '\t'), _text.end());
    //_text.erase(std::remove(_text.begin(), _text.end(), '\r'), _text.end());
    
    _tree = htmlUtils::getHTMLTree(_text);
    //htmlUtils::printTree(_tree);
}

htmlDocument::~htmlDocument()
{
    
}

elementData htmlDocument::getFirstElementMatchingPattern(const std::string &tag, const attributesMap_t &attributes) const
{
    auto occurrencies = getAllElementsMatchingPattern(tag, attributes);
    
    if (occurrencies.size() > 0)
    {
        return occurrencies.front();
    }
    else
    {
        return elementData();
    }
}

std::vector<elementData> htmlDocument::getAllElementsMatchingPattern(const std::string &tag, const attributesMap_t &attributes) const
{
    return htmlUtils::extractElementsMatchingPatternFromTree(_tree, tag, attributes);
}
