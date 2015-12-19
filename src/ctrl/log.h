/**
 * Pretty logging of application messages.
 */
#ifndef SOLEMNSKY_CTRL_LOG_H
#define SOLEMNSKY_CTRL_LOG_H

#include <string>
#include <functional>

enum class LogLevel {
    Info, Notice, Error
};

void app_log(LogLevel, std::string);

#endif //SOLEMNSKY_CTRL_LOG_H
