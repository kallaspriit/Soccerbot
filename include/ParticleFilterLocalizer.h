#ifndef PARTICLEFILTERLOCALIZER_H
#define PARTICLEFILTERLOCALIZER_H

#include "Localizer.h"
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

struct Measurement {
	Measurement() : distance(-1), angle(0) {}
	Measurement(float distance, float angle) : distance(distance), angle(angle) {}

	float distance;
	float angle;
};

typedef std::map<std::string, Landmark*> LandmarkMap;
typedef std::vector<Particle*> ParticleList;
typedef std::map<std::string, Measurement> Measurements;

class ParticleFilterLocalizer : public Localizer {
    public:
		ParticleFilterLocalizer(int particleCount = Config::ballLocalizerParticleCount, float forwardNoise = Config::ballLocalizerForwardNoise, float turnNoise = Config::ballLocalizerTurnNoise, float distanceSenseNoise = Config::ballLocalizerDistanceNoise, float angleSenseNoise = Config::ballLocalizerAngleNoise);
        ~ParticleFilterLocalizer();

        void addLandmark(Landmark* landmark);
        void addLandmark(std::string name, float x, float y);
		void move(float velocityX, float velocityY, float omega, float dt) { move(velocityX, velocityY, omega, dt, false); }
        void move(float velocityX, float velocityY, float omega, double dt, bool exact = false);
        float getMeasurementProbability(Particle* particle, const Measurements& measurements);
		void setPosition(float x, float y, float orientation);
        void update(const Measurements& measurements);
        void resample();
        Math::Position getPosition();
		const ParticleList& getParticles() const { return particles; }
		std::string getJSON() { return json; }

    private:
        const int particleCount;
        float forwardNoise;
        float turnNoise;
        float distanceSenseNoise;
        float angleSenseNoise;
        LandmarkMap landmarks;
        ParticleList particles;
		std::string json;
};

#endif // PARTICLEFILTERLOCALIZER_H
