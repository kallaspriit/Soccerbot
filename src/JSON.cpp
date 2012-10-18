#include "JSON.h"

#include <sstream>

const std::string JSON::stringify(const Properties& properties) {
    std::stringstream stream;

    stream << "{";
    bool first = true;

    for (PropertiesIterator it = properties.begin(); it != properties.end(); it++) {
        if (first != true) {
            stream << ",";
        } else {
            first = false;
        }

        stream << "\"" << it->first << "\":\"" << it->second.c_str() << "\"";
    }

    stream << "}";

    return stream.str();
}
