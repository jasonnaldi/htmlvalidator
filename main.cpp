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

#include <algorithm>
#include <unistd.h>

#include "curlUtils.hpp"
#include "fileUtils.hpp"
#include "json.hpp"
#include "htmlDocument.hpp"
#include "sharedTypedefs.hpp"
#include "shellUtils.hpp"
#include "stringUtils.hpp"
#include "urlUtils.hpp"

#define kUnknownAuthor "unknown"

#define kError "errors"
#define kWarning "warnings"
#define kNonDocumentError "non document errors"
#define kContributions "contributions"
#define kBrokenLinks "broken links"
#define kMissingImages "missing images"

using json = nlohmann::json;

int main(int argc, const char * argv[])
{
    // pwd at execution time is always ~
    std::string executablePath = fileUtils::getContainerDirectory(argv[0]);
    
    chdir(executablePath.c_str());
    
    shellUtils::clear();
    
    // Load a list of all html files, ignoring templates
    auto htmlPaths = stringUtils::stringToVector(shellUtils::exec("find . -print | grep \".html$\" | grep -v \"template\""), '\n');
    
    if (htmlPaths.size() > 0 && htmlPaths.back().compare("") == 0)
    {
        //Erase empty last line
        htmlPaths.pop_back();
    }
    
    documentsVector_t documents;

    for (ssize_t i = 0; i < htmlPaths.size(); ++i)
    {
        auto path = htmlPaths[i];
        
        std::cout << "\r";
        std::cout << "Loading .html files: " << i+1 << "/" << htmlPaths.size() << std::flush;
        
        // Source on reading files: http://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
        std::ifstream ifs(path);
        std::string htmlText((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        
        // Parse html to navigate it more easily
        htmlDocument html(htmlText);
        
        document_t document;
        
        document.path = path;
        document.author = html.getFirstElementMatchingPattern("meta", {{"name", "author"}}).attributes["content"];
        
        if (document.author.compare("") == 0)
        {
            problem_t problem;
            
            problem.type = "error";
            problem.message = "missing author";
            problem.extract = "";
            problem.firstColumn = -1; //TODO: implement
            problem.lastLine = -1;
            problem.lastColumn = -1;
            
            document.problems.push_back(problem);
        }
        
        // Inspect links
        auto links = html.getAllElementsMatchingPattern("a", {{"href", ""}}); // Look for href, just in case some links don't have it...
        auto images = html.getAllElementsMatchingPattern("img", {{"src", ""}}); // Look for href, just in case some links don't have it...
        
        links.insert(links.end(), images.begin(), images.end());
        
        for (auto &link : links)
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
            
            if (!urlUtils::isUrlValidRelativeToPath(href, executablePath + "/" + fileUtils::getContainerDirectory(path), html))
            {
                problem_t problem;
                
                problem.type = "error";
                problem.message = "broken link";
                problem.extract = href;
                problem.firstLine = 1;
                
                auto idx = htmlText.find(link.stringify);
                
                for (auto it = htmlText.begin(); it < htmlText.begin() + idx; ++it)
                {
                    if (*it == '\n')
                    {
                        ++problem.firstLine;
                    }
                }
                
                document.problems.push_back(problem);
            }
        }
        
        json result = json::parse(curlUtils::validateHTML(path));
        
        for (auto &message : result["messages"])
        {
            problem_t problem;
            
            problem.type = message["type"];
            problem.message = message["message"];
            problem.extract = message["extract"];
            // TODO: causes bug 'number' something...
            problem.lastLine = message["lastLine"];
            problem.lastColumn = message["lastColumn"];
            
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
        
        documents.push_back(document);
    }
    
    while (true)
    {
        shellUtils::clear();
        shellUtils::setColor(shellTextColor::FG_DARK_GRAY);
        std::cout << "HTML Validator v1.0.1 (November 5, 2016)" << std::endl;
        std::cout << "Developed by Jason Naldi." << std::endl;
        std::cout << std::endl;
        std::cout << "Search results by keyword(s)." << std::endl;
        std::cout << "Examples: `jason naldi`, `jason stefano valdet paolo`, `contributions`, `contributions biographies`, `kbooth', 'kbooth alovelace' and so on." << std::endl;
        std::cout << std::endl;
        std::cout << "Specifying no keyword will result in all documents being displayed." << std::endl;
        std::cout << "The keyword `exit` will terminate this program." << std::endl;
        std::cout << std::endl;
        shellUtils::resetColor();
        std::cout << "Keyword(s): ";
        std::string userInput;
        std::getline(std::cin, userInput);
        
        if (userInput.compare("exit") == 0)
        {
            break;
        }
        
        std::transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);
        
        auto args = stringUtils::stringToVector(userInput, ' ');
        
        std::map<std::string, ssize_t> aggregatedData;
        
        for (auto &document : documents)
        {
            std::string authorLowercase = document.author;
            std::transform(authorLowercase.begin(), authorLowercase.end(), authorLowercase.begin(), ::tolower);
            
            bool matches = false;
            
            for (auto &arg : args)
            {
                if (authorLowercase.find(arg) != std::string::npos ||
                    document.path.find(arg) != std::string::npos)
                {
                    matches = true;
                    break;
                }
            }
            
            if (matches)
            {
                std::cout << std::endl;
                std::cout << document.path << std::endl;
                std::cout << "Author: " << document.author << std::endl;
                
                ++aggregatedData["pages"];
                if (document.problems.size() > 0)
                {
                    for (auto &problem : document.problems)
                    {
                        ++aggregatedData[problem.type + "s"];
                        
                        if (problem.type.compare("error") == 0)
                        {
                            shellUtils::setColor(shellTextColor::FG_RED);
                        }
                        else if (problem.type.compare("info") == 0)
                        {
                            shellUtils::setColor(shellTextColor::FG_YELLOW);
                        }
                        else if (problem.type.compare("non-document-error") == 0) // TODO: right key?
                        {
                            shellUtils::setColor(shellTextColor::FG_CYAN);
                        }
                        std::cout << "\tType: " << problem.type << std::endl;
                        shellUtils::resetColor();
                        std::cout << "\tMessage: " << problem.message << std::endl;
                        std::cout << "\tExtract: ";
                        
                        shellUtils::setColor(shellTextColor::FG_BLUE);
                        std::cout << problem.extract << std::endl;
                        shellUtils::resetColor();
                        
                        std::cout << "\tLine: " << problem.firstLine << std::endl;
                        //std::cout << "\tFirst column: " << problem.firstColumn << std::endl;
                        //std::cout << "\tLast line: " << problem.lastLine << std::endl;
                        //std::cout << "\tLast column: " << problem.lastColumn << std::endl;
                        std::cout << std::endl;
                    }
                }
                else
                {
                    shellUtils::setColor(shellTextColor::FG_GREEN);
                    std::cout << "No problems found" << std::endl;
                    shellUtils::resetColor();
                }
            }
        }
        
        std::cout << std::endl;
        if (aggregatedData.size() > 0)
        {
            std::cout << "Aggregated data:" << std::endl;
            for (auto &entry : aggregatedData)
            {
                std::cout << "\t" << entry.first << ": " << entry.second << std::endl;
            }
        }
        else
        {
            std::cout << "No matching results for given keyword(s)" << std::endl;
        }
        
        std::cout << std::endl;
        
        shellUtils::waitForInput("start new search");
    }
    
    return 0;
}
