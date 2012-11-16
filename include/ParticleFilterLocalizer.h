#ifndef PARTICLEFILTERLOCALIZER_H
#define PARTICLEFILTERLOCALIZER_H

#include "Maths.h"
#include "Config.h"

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
		ParticleFilterLocalizer(int particleCount = Config::ballLocalizerParticleCount, float forwardNoise = Config::ballLocalizerForwardNoise, float turnNoise = Config::ballLocalizerTurnNoise, float senseNoise = Config::ballLocalizerSenseNoise);
        ~ParticleFilterLocalizer();

        void addLandmark(Landmark* landmark);
        void addLandmark(std::string name, float x, float y);
        void move(float velocityX, float velocityY, float omega, double dt, bool exact = false);
        float getMeasurementProbability(Particle* particle, const Measurements& measurements);
		void resetDeviation(float x, float y, float orientation);
        void update(const Measurements& measurements);
        void resample(float probabilities[], int count);
        Math::Position getPosition();
		const ParticleList& getParticles() const { return particles; }

    private:
        const int particleCount;
        float forwardNoise;
        float turnNoise;
        float senseNoise;
        LandmarkMap landmarks;
        ParticleList particles;
};

#endif // PARTICLEFILTERLOCALIZER_H
