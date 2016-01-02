/**
 * Find / access our resources directory canonically.
 *
 * Currently this is a placeholder, in the future we should have more
 * interesting resource management / deployment.
 */
#ifndef SOLEMNSKY_RESOURCES_H
#define SOLEMNSKY_RESOURCES_H

#include <string>

typedef std::string ResourceId;

std::string getIdPath(const ResourceId id);

std::string getFontPath();

#endif //SOLEMNSKY_RESOURCES_H
