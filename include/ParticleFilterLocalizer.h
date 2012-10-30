#ifndef PARTICLEFILTERLOCALIZER_H
#define PARTICLEFILTERLOCALIZER_H

#include "Maths.h"

#include <string>
#include <map>
#include <vector>

struct Landmark {
    Landmark(std::string name, float x, float y) : name(name), x(x), y(y) {}

    std::string name;
    float x;
    float y;
};

struct Particle {
    Particle(float x, float y, float orientation, float probability) : x(x), y(y), orientation(orientation), probability(probability) {}

    float x;
    float y;
    float orientation;
    float probability;
};

typedef std::map<std::string, Landmark*> LandmarkMap;
typedef std::vector<Particle*> ParticleList;
typedef std::map<std::string, float> Measurements;

class ParticleFilterLocalizer {
    public:
        ParticleFilterLocalizer();
        ~ParticleFilterLocalizer();

        void addLandmark(Landmark* landmark);
        void move(float velocityX, float velocityY, float omega, double dt);
        float getMeasurementProbability(Particle* particle, const Measurements& measurements);
        void update(const Measurements& measurements);
        void resample(float probabilities[], int count);
        Math::Position getPosition();

    private:
        const int particleCount;
        int moveNoise;
        int turnNoise;
        int senseNoise;
        LandmarkMap landmarks;
        ParticleList particles;
};

#endif // PARTICLEFILTERLOCALIZER_H
