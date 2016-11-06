//
//  urlUtils.hpp
//  htmlvalidator
//
//  Created by Jason Naldi on 05.11.16.
//  Copyright © 2016 Jason Naldi. All rights reserved.
//

#ifndef urlUtils_hpp
#define urlUtils_hpp

#include <stdio.h>
#include <string>

#include "htmlDocument.hpp"

namespace urlUtils {
    /*
     @params:
     - url: any of
        -
     */
    bool isUrlValidRelativeToPath(const std::string &url, const std::string &pwd, const htmlDocument &htmlDoc);
    
}

#endif /* urlUtils_hpp */
