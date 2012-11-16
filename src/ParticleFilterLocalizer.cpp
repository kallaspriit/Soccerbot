#include "ParticleFilterLocalizer.h"
#include "Util.h"
#include "Maths.h"
#include "Config.h"

#include <iostream>

ParticleFilterLocalizer::ParticleFilterLocalizer(int particleCount, float forwardNoise, float turnNoise, float senseNoise) : particleCount(particleCount), forwardNoise(forwardNoise), turnNoise(turnNoise), senseNoise(senseNoise) {
    for (int i = 0; i < particleCount; i++) {
        particles.push_back(new Particle(
            Math::randomFloat(0.0f, Config::fieldWidth),
            Math::randomFloat(0.0f, Config::fieldHeight),
            Math::randomFloat(0.0f, Math::TWO_PI),
            1.0f
        ));
    }
}

ParticleFilterLocalizer::~ParticleFilterLocalizer() {
    for (ParticleList::const_iterator it = particles.begin(); it != particles.end(); it++) {
        delete *it;
    }

    particles.clear();

    for (LandmarkMap::const_iterator it = landmarks.begin(); it != landmarks.end(); it++) {
        delete it->second;
    }

    landmarks.clear();
}

void ParticleFilterLocalizer::addLandmark(Landmark* landmark) {
    landmarks[landmark->name] = landmark;
}

void ParticleFilterLocalizer::addLandmark(std::string name, float x, float y) {
	addLandmark(new Landmark(name, x, y));
}

void ParticleFilterLocalizer::move(float velocityX, float velocityY, float omega, double dt, bool exact) {
	velocityX = 1.0f;
	velocityY = 0.0f;
	omega = 0.0f;

	//if (exact) {
		for (unsigned int i = 0; i < particles.size(); i++) {
			particles[i]->orientation = Math::floatModulus(particles[i]->orientation + omega * dt, Math::TWO_PI);
			particles[i]->x += (velocityX * Math::cos(particles[i]->orientation) - velocityY * Math::sin(particles[i]->orientation)) * dt;
			particles[i]->y += (velocityX * Math::sin(particles[i]->orientation) + velocityY * Math::cos(particles[i]->orientation)) * dt;
		}
	/*} else {
		float noisyVelocityX = velocityX + Math::randomGaussian(forwardNoise);
		float noisyVelocityY = velocityY + Math::randomGaussian(forwardNoise);

		for (unsigned int i = 0; i < particles.size(); i++) {
			particles[i]->orientation = Math::floatModulus(particles[i]->orientation + omega * dt + Math::randomGaussian(turnNoise) * dt, Math::TWO_PI);
			particles[i]->x += (noisyVelocityX * Math::cos(particles[i]->orientation) - noisyVelocityY * Math::sin(particles[i]->orientation)) * dt;
			particles[i]->y += (noisyVelocityX * Math::sin(particles[i]->orientation) + noisyVelocityY * Math::cos(particles[i]->orientation)) * dt;
		}
	}*/
}

void ParticleFilterLocalizer::resetDeviation(float x, float y, float orientation) {
	for (unsigned int i = 0; i < particles.size(); i++) {
		particles[i]->orientation = orientation;
        particles[i]->x = x;
        particles[i]->y = y;
		particles[i]->probability = 1;
    }
}

float ParticleFilterLocalizer::getMeasurementProbability(Particle* particle, const Measurements& measurements) {
    float probability = 1.0f;
    float measurement;
    float distance;
    std::string landmarkName;
    Landmark* landmark;
    LandmarkMap::iterator landmarkSearch;

    for (Measurements::const_iterator it = measurements.begin(); it != measurements.end(); it++) {
        landmarkName = it->first;
        measurement = it->second;
        landmarkSearch = landmarks.find(landmarkName);

        if (landmarkSearch == landmarks.end()) {
            std::cout << "- Didnt find landmark '" << landmarkName << "', this should not happen" << std::endl;

            continue;
        }

        landmark = landmarkSearch->second;
        distance = Math::distanceBetween(particle->x, particle->y, landmark->x, landmark->y);
        probability *= Math::getGaussian(distance, senseNoise, measurement);
    }

    return probability;
}

void ParticleFilterLocalizer::update(const Measurements& measurements) {
	if (measurements.size() == 0) {
		return;
	}

    Particle* particle;
    float* probabilities = new float[particles.size()];
    float maxProbability = -1;

    for (unsigned int i = 0; i < particles.size(); i++) {
        particle = particles[i];
        probabilities[i] = getMeasurementProbability(particle, measurements);

        if (maxProbability == -1 || probabilities[i] > maxProbability) {
            maxProbability = probabilities[i];
        }
    }

    for (unsigned int i = 0; i < particles.size(); i++) {
        probabilities[i] /= maxProbability;

        particles[i]->probability = probabilities[i];
    }

    resample(probabilities, particles.size());
}

void ParticleFilterLocalizer::resample(float probabilities[], int count) {
    ParticleList resampledParticles;
    Particle* particle;
    int index = Math::randomInt(0, count - 1);
    float beta = 0.0f;
    float maxProbability = 1.0f;

    for (unsigned int i = 0; i < particles.size(); i++) {
        particle = particles[i];
        beta += Math::randomFloat() * 2.0f * maxProbability;

        while (beta > probabilities[index]) {
            beta -= probabilities[index];
            index = (index + 1) % particleCount;
        }

        resampledParticles.push_back(new Particle(
            particle->x,
            particle->y,
            particle->orientation,
            particle->probability
        ));
    }

    for (ParticleList::const_iterator it = particles.begin(); it != particles.end(); it++) {
        delete *it;
    }

    particles.clear();

    particles = resampledParticles;
}

Math::Position ParticleFilterLocalizer::getPosition() {
    float xSum = 0.0f;
    float ySum = 0.0f;
    float orientationSum = 0.0f;
    unsigned int particleCount = particles.size();
    Particle* particle;

    for (unsigned int i = 0; i < particleCount; i++) {
        particle = particles[i];

        xSum += particle->x;
        ySum += particle->y;
        orientationSum += particle->orientation;
    }

    return Math::Position(
        xSum / (float)particleCount,
        ySum / (float)particleCount,
        Math::floatModulus(orientationSum / (float)particleCount, Math::TWO_PI)
    );
}
