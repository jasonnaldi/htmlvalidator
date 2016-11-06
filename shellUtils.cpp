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

#include "shellUtils.hpp"

std::string shellUtils::exec(const std::string &command)
{
    /*
     Source: http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
     */
    const char *cmd = command.c_str();
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
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
    shellUtils::exec("read -sn 1 -p \"Press any key to " + action + "...\"");
}
