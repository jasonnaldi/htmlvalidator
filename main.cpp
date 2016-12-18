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

/*
 Known bugs:
 - validator.w3.org will ban your ip if you run this program too often
 */

#include <fstream>
#include <iostream>
#include <map>
#include <thread>
#include <unistd.h>
#include <vector>

#include "curlUtils.hpp"
#include "fileUtils.hpp"
#include "shellUtils.hpp"
#include "stringUtils.hpp"
#include "urlUtils.hpp"

#define kValidatorWebsite "https://validator.w3.org/"

typedef std::map<std::string, int> statistics_t;
//typedef std::map<std::string, statistics_t> groupStatistics_t;

typedef std::map<std::string, document_t> documentsMap_t;

typedef std::vector<document_t> documentsVector_t;

int main(int argc, const char * argv[])
{
    // pwd at execution time is always ~
    auto executablePath = fileUtils::getParentDirectory(argv[0]);
    
    // set pwd to executable dir
    chdir(executablePath.c_str());
    
    shellUtils::clear();
    
    documentsMap_t documentsCache;
    
    std::map<std::string, std::string> readCache;
    
    while (true)
    {
        shellUtils::clear();
        shellUtils::setColor(shellTextColor::FG_DARK_GRAY);
        std::cout << "HTML Validator v1.2.3 (November 10, 2016)" << std::endl;
        std::cout << "Developed by Jason Naldi." << std::endl;
        std::cout << std::endl;
        std::cout << "Enter one or more keywords (case-insensitive) separated by white space to filter results." << std::endl;
        std::cout << "The program will only output data regarding the documents which contain any of the specified keywords in their author's name or in their path." << std::endl;
        std::cout << "Keyword(s) examples: `jason naldi`, `contributions`, `alovelace.html`, `jason biographies clu.html`." << std::endl;
        std::cout << std::endl;
        std::cout << "Specifying no keyword will result in all documents being displayed." << std::endl;
        std::cout << "Reserved keywords:" << std::endl;
        //std::cout << " - --force-update: will force the program to reload all cached files" << std::endl;
        std::cout << " - exit: will terminate this program." << std::endl;
        std::cout << std::endl;
        shellUtils::resetColor();
        std::cout << "Keyword(s): ";
        
        std::string userInput;
        std::getline(std::cin, userInput);
        
        // Bug fix: adding an extra white space at the end or double spaces in the program
        // caused all results to be displayed
        stringUtils::trim(userInput);
        
        std::transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);
        
        // Exit if requested.
        if (userInput.compare("exit") == 0)
        {
            break;
        }
        
        if (curlUtils::getWebsiteState(kValidatorWebsite).find("403") != std::string::npos)
        {
            shellUtils::setColor(shellTextColor::FG_RED);
            std::cout << "Too many files sent to: https://validator.w3.org/. Please try again later." << std::endl;
            shellUtils::waitForInput("exit");
            break;
        }
        else if (!curlUtils::isWebsiteOk(kValidatorWebsite))
        {
            shellUtils::setColor(shellTextColor::FG_RED);
            std::cout << "https://validator.w3.org/ unreachable. Please try again later." << std::endl;
            shellUtils::waitForInput("exit");
            break;
        }

        auto args = stringUtils::tokenize(userInput, ' ');
        
        for (auto it = args.begin(); it < args.end(); ++it)
        {
            if ((*it).compare("--force-update") == 0)
            {
                args.erase(it);
                documentsCache.clear();
            }
        }
        
        
        
        /*
         1. Prepare documents to validate
         */
        
        std::cout << "Looking for changes in the directory tree..." << std::endl;
        
        // Load a list of all html files, ignoring templates.
        auto paths = stringUtils::tokenize(shellUtils::exec("find . -print | grep \".html$\" | grep -v \"template\""), '\n');
        
        if (paths.size() > 0 && paths.back().compare("") == 0)
        {
            //Erase empty last line.
            paths.pop_back();
        }
        
        documentsMap_t searchResults;
        
        // Prepare a document per path
        for (ssize_t i = 0; i < paths.size(); ++i)
        {
            auto &path = paths[i];
            std::cout << "\r" << "Preparing file: " << i + 1 << "/" << paths.size() << std::flush;
            
            document_t document;
            
            //TODO: document_t could be a class itself...
            if (readCache.count(path) > 0)
            {
                document.plaintext = readCache[path];
            }
            else
            {
                // Source on reading files: http://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
                std::ifstream ifs(path);
                std::string htmlText((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
                ifs.close();
                stringUtils::trim(htmlText);
                
                document.plaintext = htmlText;
                
                readCache[path] = document.plaintext;
            }
            
            //Ensure white-spaces normalization
            
            document.tree = htmlUtils::parseHtmlText(document.plaintext);
            
            document.author = htmlUtils::getMetaAuthor(document.tree);
            
            std::string authorLowercase = stringUtils::lowercase(document.author);
            std::string pathLowercase = stringUtils::lowercase(path);
            
            bool matches = false;
            
            for (auto &arg : args)
            {
                if (authorLowercase.find(arg) != std::string::npos ||
                    pathLowercase.find(arg) != std::string::npos)
                {
                    matches = true;
                    break;
                }
            }
            
            if (matches || args.size() == 0)
            {
                searchResults[path] = document;
            }
        }
        
        std::cout << std::endl;
        
        
        /*
         2. Load cache
         */
        
        documentsMap_t uncachedDocuments;
        
        ssize_t i = 0;
        for (auto &searchResult : searchResults)
        {
            ++i;
            std::cout << "\r" << "Loading cache: " << i << "/" << searchResults.size() << std::flush;
            if (documentsCache.count(searchResult.first) > 0)
            {
                searchResult.second = documentsCache[searchResult.first];
            }
            else
            {
                // TODO: find less expensive way to do this list of uncached files...
                uncachedDocuments[searchResult.first] = searchResult.second;
            }
        }
        
        std::cout << std::endl;
        
        
        /*
         3. Validate documents
         */
        
        // TODO: check if load failures are common
        std::vector<std::thread>threads;
        
        for (auto &uncachedEntry : uncachedDocuments)
        {
            threads.push_back(std::thread(&htmlUtils::validateHtml, uncachedEntry.first, executablePath, std::ref(uncachedEntry.second)));
        }
        
        for (auto &thread : threads)
        {
            thread.join();
        }
        
        
        
        /*
         4. Update cache
         */
        ssize_t writingToCache = 0;
        for (auto &newlyCachedEntry : uncachedDocuments)
        {
            ++writingToCache;
            
            std::cout << "\r" << "Writing to cache: " << writingToCache << "/" << uncachedDocuments.size() << std::flush;
            
            searchResults[newlyCachedEntry.first] = newlyCachedEntry.second;
            documentsCache[newlyCachedEntry.first] = newlyCachedEntry.second;
        }
        
        
        
        /*
         5. Output data
         */
        
        std::map<std::string, ssize_t> aggregatedData;
        
        shellUtils::clear();
        
        //Display results
        for (auto &cacheEntry : searchResults)
        {
            std::cout << std::endl;
            std::cout << cacheEntry.first << std::endl;
            std::cout << "Author: " << cacheEntry.second.author << std::endl;
            
            ++aggregatedData["pages"];
            if (cacheEntry.second.problems.size() > 0)
            {
                for (auto &problem : cacheEntry.second.problems)
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
                    else if (problem.type.compare("non-document-error") == 0)
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
        
        // Output aggregated statistics.
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
