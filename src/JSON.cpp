#include "JSON.h"

#include <sstream>

const std::string JSON::stringify(const Properties& properties) {
    std::stringstream stream;

    stream << "{";

    for (PropertiesIterator it = properties.begin(); it != properties.end(); it++) {
        stream << "\"" << it->first << "\":\"" << it->second.c_str();

        if (it != properties.end()) {
            stream << "\",";
        }
    }

    stream << "}";

    return stream.str();
}
