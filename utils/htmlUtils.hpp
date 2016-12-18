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

#ifndef htmlUtils_hpp
#define htmlUtils_hpp

#include <string>
#include <unordered_map> //Order not important -> unordered_map is faster than map
#include <vector>

typedef std::unordered_map<std::string, std::string> attributesMap_t;

struct elementData
{
    std::string tag;
    std::string stringRepresentation; //The whole tag as it appears in the document
    attributesMap_t attributes;
    std::string content;
    std::vector<elementData> children;
};

typedef std::vector<elementData> elementsTree_t;

struct problem_t
{
    std::string type, message, extract;
    ssize_t firstLine, firstColumn, lastLine, lastColumn;
};

struct document_t
{
    std::string author;
    std::string plaintext;
    elementsTree_t tree;
    std::vector<problem_t> problems;
};

namespace htmlUtils
{
    /*
     @brief: given an html string, return its representation as an elementsTree_t.
     
     @param `html` A string representation of an html document.
     
     @return elementsTree_t.
     */
    elementsTree_t parseHtmlText(const std::string &html);
    
    /*
     @brief: given an html tree, return all the tags which match the the requested parameters.
     
     @param `tree` The tree representation of an html document.
     @param `tag` A specific tag to seek. Leave empty to accept all tags.
     @param `attributes` a map of attributes the found element must have. Leave an attribute's value empty to accept any value.
     
     @return elementsTree_t.
     */
    elementsTree_t extractElementsMatchingPatternFromTree(const elementsTree_t &tree, const std::string &tag, const attributesMap_t &attributes);
    
    /*
     @brief: same as above, but return only the first result.
     
     @return elementData.
     */
    elementData extractFirstElementMatchingPatternFromTree(const elementsTree_t &tree, const std::string &tag, const attributesMap_t &attributes);
    
    
    /**
     Extract the name of the author from an html tree

     @param tree A tree representation of an html document
     @return The author of the html document.
     */
    std::string getMetaAuthor(const elementsTree_t &tree);
    
    
    void validateLink(const elementData &link, const std::string &pwd, const std::string &path, document_t &document);
    
    /**
     Validate an html document

     @param path A path to the html document
     @param document A document containing the html data
     */
    void validateHtml(const std::string &path, const std::string &pwd, document_t &document);
}

#endif /* htmlUtils_hpp */
