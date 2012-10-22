#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include <map>
#include <string>

typedef std::map<int, float> LookupMap;
typedef std::map<int, float>::iterator LookupMapIt;

class LookupTable {
    public:
        inline void addValue(int key, float value) {
            map[key] = value;
        }

        bool load(std::string filename);

        float getValue(int search);

    private:
        LookupMap map;
};

#endif // LOOKUPTABLE_H
