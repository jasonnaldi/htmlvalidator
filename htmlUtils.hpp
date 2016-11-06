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

#ifndef htmlUtils_hpp
#define htmlUtils_hpp

#include <string>
#include <unordered_map> //Order not important, use faster unordered_map
#include <vector>

typedef std::unordered_map<std::string, std::string> attributesMap_t;

struct elementData
{
    std::string tag;
    std::string stringify; //The whole tag as it appears in the document
    attributesMap_t attributes;
    std::string content;
    std::vector<elementData> children;
};

typedef std::vector<elementData> elementsTree_t;

namespace htmlUtils
{
    void nextElement(const std::string &html, ssize_t &beginIdx);
    
    std::string getTag(const std::string &html, ssize_t &beginIdx);
    
    /*
     @params:
     - html: a string representing an html document
     - beginIdx: an idx from where the function will begin reading
     */
    attributesMap_t getAttributes(const std::string &html, ssize_t &beginIdx);

    std::string getContents(const std::string &html, ssize_t &beginIdx, const std::string &tag);
    
    elementsTree_t getHTMLTree(const std::string &html);
    
    /*
     @params:
     - tree: the tree representation of an html document
     - tag: a specific tag to seek. Leave empty to accept all tags
     - attribbutes: a map of attributes the found element must have. Leave an attribute's value empty
       to accept any value.
     */
    elementsTree_t extractElementsMatchingPatternFromTree(const elementsTree_t &tree, const std::string &tag, const attributesMap_t &attributes);
    
    void printTree(const elementsTree_t &tree, int indentationLevel = 0);
}

#endif /* htmlUtils_hpp */
