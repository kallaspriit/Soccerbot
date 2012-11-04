#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include <map>
#include <string>

typedef std::map<float, float> LookupMap;
typedef std::map<float, float>::iterator LookupMapIt;

class LookupTable {
    public:
        inline void addValue(float key, float value) {
            map[key] = value;
        }

        bool load(std::string filename);

        float getValue(float search);

    private:
        LookupMap map;
};

#endif // LOOKUPTABLE_H
