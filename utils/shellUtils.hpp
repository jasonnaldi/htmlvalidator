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

#ifndef shellUtils_hpp
#define shellUtils_hpp

#include <string>

enum shellTextColor
{
    FG_DEFAULT = 39,
    FG_BLACK = 30,
    FG_RED = 31,
    FG_GREEN = 32,
    FG_YELLOW = 33,
    FG_BLUE = 34,
    FG_MAGENTA = 35,
    FG_CYAN = 36,
    FG_LIGHT_GRAY = 37,
    FG_DARK_GRAY = 90,
    FG_LIGHT_RED = 91,
    FG_LIGHT_GREEN = 92,
    FG_LIGHT_YELLOW = 93,
    FG_LIGHT_BLUE = 94,
    FG_LIGHT_MAGENTA = 95,
    FG_LIGHT_CYAN = 96,
    FG_WHITE = 97,
    BG_RED = 41,
    BG_GREEN = 42,
    BG_BLUE = 44,
    BG_DEFAULT = 49
};

namespace shellUtils
{
    /*
     @brief: given a command, run it and return its output.
     
     @param `command` An executable command.
     
     @return std::string.
     */
    std::string exec(const std::string &command);
    
    /*
     @brief: given either a background or a foreground color, set it on the terminal.
    
     @param `color` Either a background or a foreground color.
     
     @return void.
     */
    void setColor(const shellTextColor &color);
    
    /*
     @brief: reset the terminal's background (white) and foreground (black) colors.
     
     @return void.
     */
    void resetColor();
    
    /*
     @brief: erase the output on the terminal, without preserving it (no ability
            to scroll back).
     
     @return void.
     */
    void clear();
    
    /*
     @brief: given a string `action`, display "press any key to " + action on the terminal
            and wait for the user to press any key.
     
     @param `action` A string to inform the user about what pressing any key will lead to.
     
     @return void.
     */
    void waitForInput(const std::string &action);
}

#endif /* shellUtils_hpp */
