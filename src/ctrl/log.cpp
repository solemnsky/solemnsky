#include <iostream>
#include <vector>
#include <sstream>
#include "log.h"

void app_log(LogType type, const std::string contents) {
    static std::vector<std::string> prefixes = {
            "INFO:   ",
            "NOTICE: ",
            "ERROR:  ",
            "        "};

    std::stringstream stream(contents);
    std::string line;

    bool isFirstLine(true);
    while(std::getline(stream, line, '\n')) {
        if (isFirstLine) {
            std::cout << prefixes[(int) type] + line + "\n";
            isFirstLine = false;
        } else {
            std::cout << prefixes[3] + line + "\n";
        }
    }
}
