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

#include <thread> //Use multithreading to drastically lower parse times

#include "curlUtils.hpp"
#include "fileUtils.hpp"
#include "htmlUtils.hpp"
#include "json.hpp"
#include "stringUtils.hpp"
#include "urlUtils.hpp"

using json = nlohmann::json;

/*
 ###############################################################################
 Static, private methods used for parsing html text.
 */

static void nextElement(const std::string &html, ssize_t &beginIdx)
{
    beginIdx = html.find("<", beginIdx);
}

static std::string getTag(const std::string &html, ssize_t &beginIdx)
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

static attributesMap_t getAttributes(const std::string &html, ssize_t &beginIdx)
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

static std::string getContents(const std::string &html, ssize_t &beginIdx, const std::string &tag)
{
    ssize_t openTagsCount = 1;
    ssize_t it = beginIdx;
    
    while (openTagsCount > 0 && it < html.length())
    {
        ++it;
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
    }
    
    if (openTagsCount > 0)
    {
        //Warning: the tag was not closed. Might be a self-closing one...
        return "";
    }
    
    // Skip the enclosing angle brackets.
    ++beginIdx;
    
    std::string result = html.substr(beginIdx, it - beginIdx);
    
    // +3 to skip `</` and `>` of the closure.
    beginIdx = it + tag.length() + 3;
    
    return result;
}

/*
 End static, private methods.
 ###############################################################################
 */

std::vector<elementData> htmlUtils::parseHtmlText(const std::string &html)
{
    std::vector<elementData> elements;
    
    ssize_t cursor = 0;
    
    while (true)
    {
        nextElement(html, cursor);
        
        if (cursor == std::string::npos)
        {
            // no more tags
            break;
        }
        
        elementData element;
        
        ssize_t cacheCursorBeginTag = cursor;
        
        element.tag = getTag(html, cursor);
        element.attributes = getAttributes(html, cursor);
        element.content = getContents(html, cursor, element.tag);
        
        element.stringRepresentation = html.substr(cacheCursorBeginTag, cursor - cacheCursorBeginTag);
        
        // Recursively parse the contents
        element.children = htmlUtils::parseHtmlText(element.content);
        
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

elementData htmlUtils::extractFirstElementMatchingPatternFromTree(const elementsTree_t &tree, const std::string &tag, const attributesMap_t &attributes)
{
    auto matchingElements = htmlUtils::extractElementsMatchingPatternFromTree(tree, tag, attributes);
    
    if (matchingElements.size() > 0)
    {
        return matchingElements.front();
    }
    else
    {
        return elementData();
    }
}

std::string htmlUtils::getMetaAuthor(const elementsTree_t &tree)
{
    auto authorElement = htmlUtils::extractFirstElementMatchingPatternFromTree(tree, "meta", {{"name", "author"}});
    return authorElement.attributes.count("content") > 0 ? authorElement.attributes["content"] : "";
}

void htmlUtils::validateLink(const elementData &link, const std::string &pwd, const std::string &path, document_t &document)
{
    std::string href = "";
    
    if (link.attributes.count("href") > 0)
    {
        // for links
        href = link.attributes.at("href");
    }
    else if (link.attributes.count("src") > 0)
    {
        // for images
        href = link.attributes.at("src");
    }
    else
    {
        // Something's wrong...
    }
    
    if (!urlUtils::isUrlValidRelativeToPath(href, pwd + "/" + fileUtils::getParentDirectory(path), document.tree))
    {
        problem_t problem;
        
        problem.type = "error";
        problem.message = "broken link";
        problem.extract = href;
        problem.firstLine = stringUtils::firstLineOccurrence(document.plaintext, link.stringRepresentation);
        
        static std::mutex writeMutex;
        writeMutex.lock();
        
        document.problems.push_back(problem);
        
        writeMutex.unlock();
    }
}

void htmlUtils::validateHtml(const std::string &path, const std::string &pwd, document_t &document)
{
    // Check author
    if (document.author.compare("") == 0)
    {
        problem_t problem;
        
        problem.type = "error";
        problem.message = "missing author";
        problem.extract = "";
        
        // -1 because it is hard to estimate where the user wants to add it...
        problem.firstLine = -1;
        problem.firstColumn = -1;
        problem.lastLine = -1;
        problem.lastColumn = -1;
        
        document.problems.push_back(problem);
    }
    
    // Inspect links
    auto links = htmlUtils::extractElementsMatchingPatternFromTree(document.tree, "a", {{"href", ""}}); // Look for href, just in case some links don't have it...
    auto images = htmlUtils::extractElementsMatchingPatternFromTree(document.tree, "img", {{"src", ""}}); // Look for href, just in case some links don't have it...
    
    links.insert(links.end(), images.begin(), images.end());
    
    /*
     // TODO: why does this sometimes fail to validate some links?
    // Check links in multiple threads because speed is cool.
    std::vector<std::thread>threads;
    
    for (auto &link : links)
    {
        threads.push_back(std::thread(&htmlUtils::validateLink, std::ref(link), pwd, path, std::ref(document)));
    }
    
    for (auto &thread : threads)
    {
        thread.join();
    }*/
    
    
    for (auto &link : links)
    {
        validateLink(link, pwd, path, document);
    }
    
    // Ensure footer author is the same as the one on the meta author tag
    auto footer = htmlUtils::extractFirstElementMatchingPatternFromTree(document.tree, "section", {{"id", "footer"}});
    auto section = htmlUtils::extractFirstElementMatchingPatternFromTree(footer.children, "div", {{"class", "container"}});
    auto footerAuthor = htmlUtils::extractFirstElementMatchingPatternFromTree(section.children, "address", {});
    
    //Bugfix: someone added &ograve; instead of ò...
    footerAuthor.content = stringUtils::replaceAllOccurrencies(footerAuthor.content, "&ograve;", "ò");
    
    //Some people added their footer name inside a link...
    auto linkData = htmlUtils::extractFirstElementMatchingPatternFromTree(footerAuthor.children, "a", {});
    
    if (footerAuthor.content.compare("") == 0)
    {
        problem_t problem;
        
        problem.type = "error";
        problem.message = "missing footer author";
        problem.extract =
        "\n\t\t<section id=\"footer\">\n"
        "\t\t\t<div class=\"container\">\n"
        "\t\t\t\t<h4>Author</h4>\n"
        "\t\t\t\t<address>Your Name</address>\n"
        "\t\t\t</div>\n"
        "\t\t</section>";
        
        // -1 because it is hard to estimate where the user wants to add it...
        problem.firstLine = -1;
        problem.firstColumn = -1;
        problem.lastLine = -1;
        problem.lastColumn = -1;
        
        document.problems.push_back(problem);
    }
    else if (footerAuthor.content.compare(document.author) != 0 && linkData.content.compare(document.author) != 0)
    {
        problem_t problem;
        
        problem.type = "error";
        problem.message = "mismatch between author specified in meta and footer";
        problem.extract = "\"" + document.author + "\" vs. \"" + (linkData.content.compare("") != 0 ? linkData.content : footerAuthor.content) + "\"";
        problem.firstLine = stringUtils::firstLineOccurrence(document.plaintext, footerAuthor.stringRepresentation);
        problem.firstColumn = problem.firstLine;
        problem.lastLine = problem.firstLine;
        problem.lastColumn = problem.firstLine;
        
        document.problems.push_back(problem);
    }
    
    std::string response = curlUtils::validateHTML(path);
    
    if (response.length() == 0)
    {
        return;
    }
    
    json result = json::parse(response);
    
    if (result.count("messages") > 0)
    {
        for (auto &message : result["messages"])
        {
            problem_t problem;
            
            if (message.count("type") > 0)
            {
                problem.type = message["type"];
            }
            else
            {
                problem.type = "unknown";
            }
            
            if (message.count("message") > 0)
            {
                problem.message = message["message"];
            }
            else
            {
                problem.message = "No message";
            }
            
            if (message.count("extract") > 0)
            {
                problem.extract = message["extract"];
            }
            else
            {
                problem.extract = "No extract";
            }
            
            if (message.count("lastLine") > 0)
            {
                problem.lastLine = message["lastLine"];
            }
            else
            {
                problem.lastLine = -1;
            }
            
            if (message.count("lastColumn") > 0)
            {
                problem.lastColumn = message["lastColumn"];
            }
            else
            {
                problem.lastColumn = -1;
            }
            
            if (message.count("firstLine") > 0)
            {
                problem.firstLine = message["firstLine"];
            }
            else
            {
                problem.firstLine = problem.lastLine;
            }
            
            if (message.count("firstColumn") > 0)
            {
                problem.firstColumn = message["firstColumn"];
            }
            else
            {
                problem.firstColumn = problem.lastColumn;
            }
            
            document.problems.push_back(problem);
        }
    }
    
    static std::mutex writeMutex;
    static ssize_t filesCount = 0;
    
    writeMutex.lock();
    
    ++filesCount;
    std::cout << "\r" << "Validating files: " << filesCount << std::flush;
    
    writeMutex.unlock();
}
