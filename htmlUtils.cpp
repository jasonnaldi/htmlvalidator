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

#include "htmlUtils.hpp"

#include <iostream>

void htmlUtils::nextElement(const std::string &html, ssize_t &beginIdx)
{
    beginIdx = html.find("<", beginIdx);
}

std::string htmlUtils::getTag(const std::string &html, ssize_t &beginIdx)
{
    std::string tag;
    
    // +1 to skip the initial `<`
    for (auto it = html.begin() + beginIdx + 1; it < html.end(); ++it)
    {
        if (*it == ' ' || *it == '>' || *it == '\\')
        {
            // A space indicates the end of a tag
            // In case of `>`, it is a self-closing tag with no attributes, like <br> or <br/>
            break;
        }
        
        tag += *it;
        
        //Keep beginIdx up to date
        ++beginIdx;
    }
    
    return tag;
}

attributesMap_t htmlUtils::getAttributes(const std::string &html, ssize_t &beginIdx)
{
    attributesMap_t attributes;
    
    bool isReadingKey = true;
    std::string tempKey = "";
    std::string tempValue = "";
    char contentEnclosingQuoteChar = 0x00;
    
    // std::string::find could have been used here, but
    // parsing char by char is easier to read.
    
    // tags are structured as follows: `key="value" key2='value2' ... keyn="valuen"`
    for (auto it = html.begin() + beginIdx + 2; it < html.begin() + html.find(">", beginIdx); ++it)
    {
        if (*it == '=' && isReadingKey)
        {
            // End of `key`, move to `value`
            isReadingKey = false;
            ++it; //skip the `"` or `'` after `=`
            contentEnclosingQuoteChar = *it;
        }
        else if ((*it == contentEnclosingQuoteChar) && tempValue.length() > 0)
        {
            // End of `value`, add the pair key-value and move to the next
            isReadingKey = true;
            
            attributes[tempKey] = tempValue;
            
            tempKey = tempValue = "";
            
            ++it; // skip the trailing space
        }
        else if ((*it == '/' && *(it + 1) == '>') || *it == '>')
        {
            //The tag is over. exit
            break;
        }
        else if (isReadingKey)
        {
            tempKey += *it;
        }
        else
        {
            tempValue += *it;
        }
    }
    
    // Go to the close bracket of the opening tag
    beginIdx = html.find(">", beginIdx);
    
    return attributes;
}

std::string htmlUtils::getContents(const std::string &html, ssize_t &beginIdx, const std::string &tag)
{
    ssize_t openTagsCount = 1;
    ssize_t it = beginIdx;
    
    while (openTagsCount > 0 && it < html.length())
    {
        if (html[it] == '<')
        {
            //`</tag...`?
            if (html.substr(it, tag.length() + 2).compare("</" + tag) == 0)
            {
                --openTagsCount;
            }
            else if (html.substr(it, tag.length() + 1).compare("<" + tag) == 0)
            {
                ++openTagsCount;
            }
        }
        
        ++it;
    }
    
    if (openTagsCount > 0)
    {
        //Warning: the tag was not closed. Might be a self-closing one...
        return "";
    }
    
    std::string result = html.substr(beginIdx, it - beginIdx);
    
    // +3 to skip `</` and `>` of the closure.
    beginIdx = it + tag.length() + 3;
    
    return result;
}

std::vector<elementData> htmlUtils::getHTMLTree(const std::string &html)
{
    std::vector<elementData> elements;
    
    ssize_t cursor = 0;
    
    while (true)
    {
        htmlUtils::nextElement(html, cursor);
        
        if (cursor == std::string::npos)
        {
            // no more tags
            break;
        }
        
        elementData element;
        
        ssize_t cacheCursorBeginTag = cursor;
        
        element.tag = htmlUtils::getTag(html, cursor);
        element.attributes = htmlUtils::getAttributes(html, cursor);
        element.content = htmlUtils::getContents(html, cursor, element.tag);
        
        element.stringify = html.substr(cacheCursorBeginTag, cursor - cacheCursorBeginTag);
        
        // Recursively parse the contents
        element.children = htmlUtils::getHTMLTree(element.content);
        
        elements.push_back(element);
    }
    
    return elements;
}

elementsTree_t htmlUtils::extractElementsMatchingPatternFromTree(const elementsTree_t &tree, const std::string &tag, const attributesMap_t &attributes)
{
    elementsTree_t results;
    
    for (auto &element : tree)
    {
        if (tag.length() == 0 || element.tag.compare(tag) == 0)
        {
            bool equivalent = true;
            
            for (auto &attribute : attributes)
            {
                // Check that all attributes have the requested value.
                // If no value has been specified (""), then all values are accepted.
                if (element.attributes.count(attribute.first) == 0 ||
                    (attribute.second.length() > 0 &&
                     attribute.second.compare(element.attributes.at(attribute.first)) != 0))
                {
                    equivalent = false;
                }
            }
            
            if (equivalent)
            {
                results.push_back(element);
            }
        }
        
        auto childrenResults = htmlUtils::extractElementsMatchingPatternFromTree(element.children, tag, attributes);
        results.insert(results.end(), childrenResults.begin(), childrenResults.end());
    }
    
    return results;
}

void htmlUtils::printTree(const elementsTree_t &tree, int indentationLevel)
{
    for (auto &element : tree)
    {
        for (int i = 0; i < indentationLevel; ++i)
        {
            std::cout << " ";
        }
        std::cout << "<" << element.tag << " ";
        
        for (auto &attribute : element.attributes)
        {
            std::cout << attribute.first << "=\"" << attribute.second << "\" ";
        }
        
        std::cout << ">";
        
        std::cout << std::endl;
        
        printTree(element.children, indentationLevel + 3);
    }
}
