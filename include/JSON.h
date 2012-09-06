#ifndef JSON_H
#define JSON_H

#include <string>
#include <map>

typedef std::map<std::string, std::string> Properties;
typedef std::map<std::string, std::string>::const_iterator PropertiesIterator;

class JSON {
    public:
        static const std::string stringify(const Properties& properties);

};

#endif // JSON_H
