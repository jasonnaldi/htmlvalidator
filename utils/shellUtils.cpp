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

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "shellUtils.hpp"

std::string shellUtils::exec(const std::string &command)
{
    /*
     Source: http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
     */
    
    // Bug fix: the program spawns a progressive amount of threads. Make sure not to go over
    // the limit of open files.
    const char *cmd = command.c_str();
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (pipe)
    {
        while (!feof(pipe.get()))
        {
            if (fgets(buffer, 128, pipe.get()) != NULL)
                result += buffer;
        }
        return result;
    }
    else
    {
        // Wait for a while. In the meantime, another program might end.
        // Then, retry.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //throw std::runtime_error("popen() failed!");
        return shellUtils::exec(command);
    }
}

void shellUtils::setColor(const shellTextColor &color)
{
    /*
     Source: http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
     */
    std::cout << "\033[" << color << "m";
}

void shellUtils::resetColor()
{
    setColor(shellTextColor::FG_DEFAULT);
    setColor(shellTextColor::BG_DEFAULT);
}

void shellUtils::clear()
{
    /*
     Source: http://askubuntu.com/questions/25077/how-to-really-clear-the-terminal
     */
    std::system("clear && printf '\e[3J'");
}

void shellUtils::waitForInput(const std::string &action)
{
    /*
     Source: http://www.commandlinefu.com/commands/view/3992/press-any-key-to-continue
     */
    shellUtils::exec("read -sn 1 -p \"Press any key to " + action + "...\n\"");
}
