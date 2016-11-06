//
//  urlUtils.cpp
//  htmlvalidator
//
//  Created by Jason Naldi on 05.11.16.
//  Copyright © 2016 Jason Naldi. All rights reserved.
//

#include "urlUtils.hpp"

#include "curlUtils.hpp"
#include "fileUtils.hpp"
#include "shellUtils.hpp"

bool urlUtils::isUrlValidRelativeToPath(const std::string &url, const std::string &pwd, const htmlDocument &htmlDoc)
{
    bool available = false;
    
    // Foud kinds of url:
    // Website
    if (url.substr(0, 4).compare("http") == 0 || url.substr(0, 3).compare("www") == 0)
    {
        available = !curlUtils::doesWebsite404(url);
    }
    // Internal anchor
    else if (url.front() == '#')
    {
        auto foundTagsWithLinkId = htmlDoc.getAllElementsMatchingPattern("", {{"id", url.substr(1, url.length() - 1)}});
        
        available = foundTagsWithLinkId.size() > 0;
    }
    // For pages on local machine, remember that the doc might be on a different dir than this executable.
    // External page on local machine, no anchor
    else if (url.find("#") == std::string::npos)
    {
        std::string pathRelativeToExecutable = pwd + "/" + url;
        available = fileUtils::doesFileExist(pathRelativeToExecutable);
    }
    else
    {
        ssize_t hashIdx = url.find("#");
        // No need to check for npos hashIdx, since it has already been done in the previous case
        
        std::string linkPath = url.substr(0, hashIdx);
        linkPath = pwd + linkPath;
        
        std::string itemId = url.substr(hashIdx + 1, url.length() - hashIdx - 1);
        
        available = fileUtils::doesFileExist(linkPath);
        
        // Do not look for anchor if file does not exist
        if (available)
        {
            // Use `grep` for shortness. If it outputs nothing, it did not find the id
            std::string grepCmd = "grep 'id=\"" + itemId + "\"' " + linkPath;
            available = shellUtils::exec(grepCmd).length() > 0;
        }
    }
    
    return available;
}
