/**
 * Pretty logging of application messages.
 */
#ifndef SOLEMNSKY_CTRL_LOG_H
#define SOLEMNSKY_CTRL_LOG_H

#include <string>
#include <functional>

enum class LogType {
    Info, Notice, Error
};

void app_log(LogType type, const std::string contents);

#endif //SOLEMNSKY_CTRL_LOG_H
