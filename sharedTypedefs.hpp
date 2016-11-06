//
//  sharedTypedefs.hpp
//  htmlvalidator
//
//  Created by Jason Naldi on 05.11.16.
//  Copyright © 2016 Jason Naldi. All rights reserved.
//

#ifndef sharedTypedefs_hpp
#define sharedTypedefs_hpp

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, int> statistics_t;
typedef std::map<std::string, statistics_t> groupStatistics_t;

typedef struct
{
    std::string type, message, extract;
    ssize_t firstLine, firstColumn, lastLine, lastColumn;
} problem_t;

typedef std::string path_t;

typedef struct
{
    std::string author;
    std::string path;
    std::vector<problem_t> problems;
} document_t;

typedef std::vector<document_t> documentsVector_t;

#endif /* sharedTypedefs_hpp */
