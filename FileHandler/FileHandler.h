#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>

class FileHandler
{
public:
    static std::string loadFile(std::string);
    static void outputFile(std::string, std::string);
};

#endif