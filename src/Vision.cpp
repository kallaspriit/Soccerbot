#include "Vision.h"
#include "Config.h"
#include "Maths.h"

#include <iostream>
#include <algorithm>

Vision::Vision(int width, int height) : blobber(NULL), width(width), height(height), lastFrame(NULL), classification(NULL) {
    blobber = new Blobber();

    blobber->initialize(width, height);
    blobber->loadOptions(Config::blobberConfigFilename);
    blobber->enable(BLOBBER_DENSITY_MERGE);

    frontDistanceLookup.load("config/distance-front.cfg"/*, 0.13f*/);
    rearDistanceLookup.load("config/distance-rear.cfg");
    frontAngleLookup.load("config/angle-front.cfg");
    rearAngleLookup.load("config/angle-rear.cfg");

    validBallBgColors.push_back("green");
    validBallBgColors.push_back("white");
    validBallBgColors.push_back("black");
    validBallBgColors.push_back("ball");
    validBallBgColors.push_back("yellow-goal");
    validBallBgColors.push_back("blue-goal");

    validBallPathColors.push_back("green");
    validBallPathColors.push_back("white");
    validBallPathColors.push_back("black");
    validBallPathColors.push_back("ball");
    validBallPathColors.push_back("yellow-goal");
    validBallPathColors.push_back("blue-goal");

    validGoalPathColors.push_back("green");
    validGoalPathColors.push_back("white");
    validGoalPathColors.push_back("black");
    validGoalPathColors.push_back("ball");
    validGoalPathColors.push_back("yellow-goal");
    validGoalPathColors.push_back("blue-goal");

    //blobber->enable(BLOBBER_DUAL_THRESHOLD);
    //blobber->setMapFilter(this);

    /*
    [Colors]
    (255,128,  0) 0.5000 5 ball
    (255,255,  0) 0.5000 5 yellow-goal
    (  0,  0,255) 0.5000 5 blue-goal
    (255,255,255) 0.5000 5 white
    (  0,255,  0) 0.5000 5 green
    ( 32, 32, 32) 0.5000 5 black

    [Thresholds]
    ( 50:133, 88:128,154:196)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    */

    /*
    frontDistanceLookup.load("config/distance-front.cfg");

    for (int i = 0; i < 800; i += 100) {
        float v = frontDistanceLookup.getValue(i);

        std::cout << "Test " << i << ": " << v << std::endl;
    }
    */

	/*LookupTable testLookup;
    testLookup.addValue(0, 4.0f);
    testLookup.addValue(100, 3.0f);
    testLookup.addValue(150, 2.0f);

    double d1 = testLookup.getValue(0);
    double d2 = testLookup.getValue(100);
    double d3 = testLookup.getValue(50);
    double d4 = testLookup.getValue(25);
    double d5 = testLookup.getValue(75);
    double d6 = testLookup.getValue(125);
    double d7 = testLookup.getValue(200);

    std::cout << "Distance   0: " << d1 << " / 4.00" << std::endl;
    std::cout << "Distance 100: " << d2 << " / 3.00" << std::endl;
    std::cout << "Distance  50: " << d3 << " / 3.50" << std::endl;
    std::cout << "Distance  25: " << d4 << " / 3.75" << std::endl;
    std::cout << "Distance  75: " << d5 << " / 3.25" << std::endl;
    std::cout << "Distance 125: " << d6 << " / 2.50" << std::endl;
    std::cout << "Distance 200: " << d7 << " / 2.00" << std::endl;*/
}

Vision::~Vision() {
    if (blobber != NULL) {
        blobber->saveOptions(Config::blobberConfigFilename);

        delete blobber;

        blobber = NULL;
    }

    if (classification != NULL) {
        delete classification;
    }
}

void Vision::setFrame(unsigned char* frame) {
    blobber->processFrame((Blobber::Pixel*)frame);

    lastFrame = frame;
}

void Vision::process(Dir dir) {
    processBalls(dir);
	processGoals(dir);
}

void Vision::processBalls(Dir dir) {
	ObjectList* balls = dir == Dir::DIR_FRONT ? &frontBalls : &rearBalls;

    for (ObjectListIt it = balls->begin(); it != balls->end(); it++) {
        delete *it;
    }

    balls->clear();

    float distance;
    float angle;

    Blobber::Blob* blob = blobber->getBlobs("ball");

    while (blob != NULL) {
		distance = getDistance(dir, blob->centerX, blob->y2);
        angle = getAngle(dir, blob->centerX, blob->y2);

        Object* ball = new Object(
            blob->centerX,
            blob->centerY,
            blob->x2 - blob->x1,
            blob->y2 - blob->y1,
            blob->area,
            distance,
            angle
        );

        if (isValidBall(ball)) {
            balls->push_back(ball);
        }

        blob = blob->next;
    }
}

void Vision::processGoals(Dir dir) {
	ObjectList* goals = dir == Dir::DIR_FRONT ? &frontGoals : &rearGoals;

    for (ObjectListIt it = goals->begin(); it != goals->end(); it++) {
        delete *it;
    }

    goals->clear();

    float distance;
    float angle;

    for (int i = 0; i < 2; i++) {
        Blobber::Blob* blob = blobber->getBlobs(i == 0 ? "yellow-goal" : "blue-goal");

        while (blob != NULL) {
			distance = getDistance(dir, blob->centerX, blob->y2);
            angle = getAngle(dir, blob->centerX, blob->y2);

            Object* goal = new Object(
                blob->centerX,
                blob->centerY,
                blob->x2 - blob->x1,
                blob->y2 - blob->y1,
                blob->area,
                distance,
                angle,
                i == 0 ? Side::YELLOW : Side::BLUE
            );

            if (isValidGoal(goal, i == 0 ? Side::YELLOW : Side::BLUE)) {
                goals->push_back(goal);
            }

            blob = blob->next;
        }
    }
}

bool Vision::isValidBall(Object* ball) {
    if (ball->area < Config::ballMinArea) {
        return false;
    }

    int ballRadius = Math::max(ball->width, ball->height) / 2;
    int senseRadius = ballRadius * 1.35f * Math::max(ball->distance / 2.0f, 1.0f) + 10.0f;

    float surroundMetric = getSurroundMetric(
        ball->x,
        ball->y,
        senseRadius,
        validBallBgColors,
		"green"
    );

    //std::cout << "Surround: " << surroundMetric << std::endl;

    if (surroundMetric < Config::validBallSurroundThreshold) {
        return false;
    }

    PathMetric pathMetric = getPathMetric(
        Config::cameraPathStartX,
        Config::cameraPathStartY,
        ball->x,
        ball->y,
        validBallPathColors
        //,"green"
    );

    //std::cout << "Ball path: " << pathMetric << std::endl;

	if (
		pathMetric.percentage < Config::validBallPathThreshold
		//|| !pathMetric.validColorFound
		|| pathMetric.invalidSpree > getBallMaxInvalidSpree(ball->y + ball->height / 2)
	) {
        return false;
    }

    return true;
}

int Vision::getBallMaxInvalidSpree(int y) {
	return y / Math::pow(y, Config::goalInvalidSpreeScaler) + Config::ballMinInvalidSpree;
}

int Vision::getGoalMaxInvalidSpree(int y) {
	return y / Math::pow(y, Config::goalInvalidSpreeScaler) + Config::goalMinInvalidSpree;
}

bool Vision::isValidGoal(Object* goal, int side) {
    /*PathMetric pathMetric = getPathMetric(
        Config::cameraPathStartX,
        Config::cameraPathStartY,
        goal->x,
        goal->y,
        validGoalPathColors
    );*/

	/*if (side == Side::YELLOW) {
		float blockMetric = getBlockMetric(
			goal->x - goal->width / 2,
			goal->y + goal->height / 2,
			goal->width,
			goal->height,
			validGoalPathColors
		);

		std::cout << "! Goal block: " << blockMetric << std::endl;
	} else {*/
		float undersideMetric = getUndersideMetric(
			goal->x - goal->width / 2,
			goal->y - goal->height / 2,
			goal->width,
			goal->height,
			side == Side::YELLOW ? "yellow-goal" : "blue-goal",
			validGoalPathColors
		);

		if (undersideMetric < Config::goalMinUndersideMetric) {
			return false;
		}

		//std::cout << "! Goal underside: " << undersideMetric << std::endl;
	//}

	/*if (
		pathMetric.percentage < Config::validGoalPathThreshold
		//|| !pathMetric.validColorFound
		|| pathMetric.invalidSpree > getBallMaxInvalidSpree(goal->y + goal->height / 2)
	) {
        return false;
    }*/

    return true;
}

/*void Vision::filterMap(unsigned int* map) {

}*/

float Vision::getDistance(Dir dir, int x, int y) {
	//float yCorrection = 0.000095 * Math::pow(x, 2) - 0.0536 * x + 7;
	//std::cout << "! Y-correction: " << yCorrection << " at x: " << x << std::endl;

	float yCorrection = 0;

    if (dir == DIR_FRONT) {
		return frontDistanceLookup.getValue(y - yCorrection);
    } else {
        return rearDistanceLookup.getValue(y - yCorrection);
    }
}

float Vision::getHorizontalDistance(Dir dir, int x, int y) {
	/*float measurementPixels = 150;
	float distance = getDistance(dir, x, y);
	float distanceLookup = dir == DIR_FRONT ? frontAngleLookup.getValue(distance) : frontAngleLookup.getValue(distance);
	float metersPerPixel = distanceLookup / measurementPixels;
	float centerOffset = (float)(x - (Config::cameraWidth / 2));
	float horizontalDistance = centerOffset * metersPerPixel;*/

	/*std::cout << "! Solve horizontal distance for " << x << "x" << y << std::endl;
	std::cout << " > Dir " << dir << std::endl;
	std::cout << " > Distance " << distance << std::endl;
	std::cout << " > Horizontal " << measurementPixels << " pixels meters: " << distanceLookup << std::endl;
	std::cout << " > Test 2m lookup: " << frontAngleLookup.getValue(2.0f) << std::endl;
	std::cout << " > Meters for pixel: " << metersPerPixel << std::endl;
	std::cout << " > Center offset: " << centerOffset << std::endl;
	std::cout << " > Horizontal distance: " << horizontalDistance << std::endl;*/

	//return horizontalDistance;

	float distance = getDistance(dir, x, y);
	float centerOffset = (float)(x - (Config::cameraWidth / 2));
	float localAngle = distance / 686.0f;

	return Math::tan(localAngle) * (y + 0.062);
}

float Vision::getAngle(Dir dir, int x, int y) {
    /*float centerOffset = (float)(x - (Config::cameraWidth / 2));
    float distance = getDistance(dir, x, y);
    float pixelsPerCm = dir == DIR_FRONT ? frontAngleLookup.getValue(distance) : rearAngleLookup.getValue(distance);
    float horizontalDistance = (double)centerOffset / pixelsPerCm;
	//return Math::tan(horizontalDistance / distance) * 180.0 / Math::PI;
	*/

	/*float distance = getDistance(dir, x, y);
	float horizontalDistance = getHorizontalDistance(dir, x, y);
	return Math::tan(horizontalDistance / distance);*/

	//float distance = getDistance(dir, x, y);

	// last working
	float centerOffset = (float)(x - (Config::cameraWidth / 2));
	
	return Math::degToRad(centerOffset / 11.5f);

	/*float distance = getDistance(dir, x, y);
	float horizontalDistance = getHorizontalDistance(dir, x, y);

	return Math::atan(horizontalDistance / distance);*/
}

Blobber::Color* Vision::getColorAt(int x, int y) {
    return blobber->getColorAt(x, y);
}

float Vision::getSurroundMetric(int x, int y, float radius, std::vector<std::string> validColors, std::string requiredColor) {
    int matches = 0;
    int points = radius;
    bool requiredColorFound = false;
    bool debug = img.data != NULL;

    for (int i = 0; i < points; i++) {
        double t = 2 * Math::PI * i / points + Math::PI;

        int senseX = x + radius * cos(t);
        int senseY = y + radius * sin(t);

        Blobber::Color* color = getColorAt(senseX, senseY);

        if (color != NULL) {
            if (find(validColors.begin(), validColors.end(), std::string(color->name)) != validColors.end()) {
                matches++;

                if (debug) {
                    img.drawMarker(senseX, senseY, 0, 200, 0);
                }
            }

            if (requiredColor != "" && color->name == requiredColor) {
                requiredColorFound = true;
            }
        } else {
            if (debug) {
                img.drawMarker(senseX, senseY, 200, 0, 0);
            }
        }
    }

    if (requiredColor != "" && !requiredColorFound) {
        return 0.0f;
    } else {
        return (float)matches / (float)points;
    }
}

Vision::PathMetric Vision::getPathMetric(int x1, int y1, int x2, int y2, std::vector<std::string> validColors, std::string requiredColor) {
	x1 = Math::limit(x1, 0, Config::cameraWidth);
	x2 = Math::limit(x2, 0, Config::cameraWidth);
	y1 = Math::limit(y1, 0, Config::cameraHeight);
	y2 = Math::limit(y2, 0, Config::cameraHeight);
	
	if (y2 > y1) {
		return PathMetric(0.0f, 0, false);
	}
	
	int F, x, y;
    int pixelCounter = 0;
    int senseCounter = 0;
    int senseStep = 3;
    const int maxSensePoints = 255;
    int senseX[maxSensePoints];
    int senseY[maxSensePoints];
	int invalidSpree = 0;
	int longestInvalidSpree = 0;
	//int scaler = 10;

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    if (x1 == x2) {
        if (y1 > y2) {
            std::swap(y1, y2);
        }

        x = x1;
        y = y1;

        while (y <= y2) {
			//senseStep = (y + scaler) / scaler;

            if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                senseX[senseCounter] = x;
                senseY[senseCounter] = y;
                senseCounter++;
            }

            pixelCounter++;

            y++;
        }
    } else if (y1 == y2) {
        x = x1;
        y = y1;

		//senseStep = (y + scaler) / scaler;

        while (x <= x2) {
            if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                senseX[senseCounter] = x;
                senseY[senseCounter] = y;
                senseCounter++;
            }

            pixelCounter++;

            x++;
        }
    } else {
        int dy = y2 - y1;
        int dx = x2 - x1;
        int dy2 = (dy << 1);
        int dx2 = (dx << 1);
        int sub = dy2 - dx2;
        int sum = dy2 + dx2;

        if (dy >= 0) {
            if (dy <= dx) {
                F = dy2 - dx;
                x = x1;
                y = y1;

                while (x <= x2) {
					//senseStep = (y + scaler) / scaler;

                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F += dy2;
                    } else {
                        y++;
                        F += sub;
                    }

                    x++;
                }
            } else {
                F = dx2 - dy;
                y = y1;
                x = x1;

                while (y <= y2) {
					//senseStep = (y + scaler) / scaler;

                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F += dx2;
                    } else {
                        x++;
                        F -= sub;
                    }

                    y++;
                }
            }
        } else {
            if (dx >= -dy) {
                F = -dy2 - dx;
                x = x1;
                y = y1;

                while (x <= x2) {
					//senseStep = (y + scaler) / scaler;

                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F -= dy2;
                    } else {
                        y--;
                        F -= sum;
                    }

                    x++;
                }
            } else {
                F = dx2 + dy;
                y = y1;
                x = x1;

                while (y >= y2) {
					//senseStep = (y + scaler) / scaler;

                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F += dx2;
                    } else {
                        x++;
                        F += sum;
                    }

                    y--;
                }
            }
        }
    }

    int matches = 0;
    bool debug = img.data != NULL;
    bool requiredColorFound = false;

    for (int i = 0; i < senseCounter; i++) {
        x = senseX[i];
        y = senseY[i];

        Blobber::Color* color = getColorAt(x, y);

        if (color != NULL) {
            if (find(validColors.begin(), validColors.end(), std::string(color->name)) != validColors.end()) {
                matches++;

				if (invalidSpree > longestInvalidSpree) {
					longestInvalidSpree = invalidSpree;
				}

				invalidSpree = 0;

                if (debug) {
                    img.drawMarker(x, y, 0, 200, 0);
                }
            } else {
				invalidSpree++;

				if (debug) {
                    img.drawMarker(x, y, 200, 0, 0);
                }
			}

            if (requiredColor != "" && color->name == requiredColor) {
                requiredColorFound = true;
            }
        } else {
            if (debug) {
                img.drawMarker(x, y, 200, 0, 0);
            }

			invalidSpree++;
        }
    }

	float percentage = (float)matches / (float)senseCounter;
	bool validColorFound = requiredColor == "" || requiredColorFound;

	return PathMetric(percentage, longestInvalidSpree, validColorFound);
}

float Vision::getBlockMetric(int x1, int y1, int blockWidth, int blockHeight, std::vector<std::string> validColors) {
	bool debug = img.data != NULL;
	int step = 6;
	int matches = 0;
	int misses = 0;

	for (int x = x1; x < x1 + blockWidth; x += step) {
		for (int y = y1; y < y1 + blockHeight; y += step) {
			Blobber::Color* color = getColorAt(x, y);

			if (color != NULL) {
				if (find(validColors.begin(), validColors.end(), std::string(color->name)) != validColors.end()) {
					matches++;

					if (debug) {
						img.drawMarker(x, y, 0, 200, 0);
					}
				} else {
					misses++;

					if (debug) {
						img.drawMarker(x, y, 200, 0, 0);
					}
				}
			} else {
				misses++;

				if (debug) {
					img.drawMarker(x, y, 200, 0, 0);
				}
			}
		}
	}

	int points = matches + misses;

	return (float)matches / (float)points;
}

float Vision::getUndersideMetric(int x1, int y1, int blockWidth, int blockHeight, std::string targetColor, std::vector<std::string> validColors) {
	bool debug = img.data != NULL;
	int xStep = 6;
	int yStep = 6;
	int gapStep = 3;
	int senseSteps = 10;
	int matches = 0;
	int misses = 0;
	int stepsBelow;
	const char* targetColorName = targetColor.c_str();
	Blobber::Color* color;

	for (int x = x1; x < x1 + blockWidth; x += xStep) {
		stepsBelow = 0;

		for (int y = y1; y < Math::min(y1 + blockHeight * 4, height); y += yStep) {
			color = getColorAt(x, y);

			if (color != NULL) {
				if (strcmp(color->name, targetColorName) == 0) {
					if (debug) {
						img.drawMarker(x, y, 255, 255, 255);
					}

					//std::cout << "! DOWN FROM " << (y + yStep) << " to " << y + blockHeight << std::endl;

					for (int senseY = y + yStep; senseY < Math::min(y + blockHeight, height); senseY += yStep) {
						color = getColorAt(x, senseY);

						//std::cout << "! SENSE " << x << " " << senseY << " | " << blockHeight << std::endl;

						if (color != NULL) {
							if (strcmp(color->name, targetColorName) == 0) {
								if (debug) {
									img.drawMarker(x, senseY, 0, 0, 0);
								}
							} else {
								for (int gapY = senseY; gapY < Math::min(senseY + senseSteps * yStep, height); gapY += gapStep) {
									color = getColorAt(x, gapY);

									if (color != NULL) {
										if (find(validColors.begin(), validColors.end(), std::string(color->name)) != validColors.end()) {
											matches++;

											if (debug) {
												img.drawMarker(x, gapY, 0, 200, 0, true);
											}
										} else {
											misses++;

											if (debug) {
												img.drawMarker(x, gapY, 200, 0, 0, true);
											}
										}
									} else {
										img.drawMarker(x, gapY, 100, 0, 0, true);

										misses++;
									}
								}

								break;
							}
						} else {
							if (debug) {
								img.drawMarker(x, senseY, 100, 0, 0);
							}

							//misses++;
						}
					}

					break;
				}
			} else {
				if (debug) {
					img.drawMarker(x, y, 200, 200, 200);
				}
			}
		}
	}

	int points = matches + misses;

	if (points == 0) {
		return 1;
	} else {
		return (float)matches / (float)points;
	}
}

ImageBuffer* Vision::classify() {
    if (lastFrame == NULL) {
        return NULL;
    }

    if (classification == NULL) {
        classification = new unsigned char[width * height * 3];
    }

    blobber->classify((Blobber::Rgb*)classification, (Blobber::Pixel*)lastFrame);

    img.width = width;
    img.height = height;
    img.data = classification;

    return &img;
}

Object* Vision::getClosestBall() {
	const ObjectList& frontBalls = getFrontBalls();
	float closestDistance;
	Object* ball;
	Object* closestBall = NULL;

	for (ObjectListItc it = frontBalls.begin(); it != frontBalls.end(); it++) {
		ball = *it;
		
		if (closestBall == NULL || ball->distance < closestDistance) {
			closestBall = ball;
			closestDistance = ball->distance;
		}
	}

	const ObjectList& rearBalls = getRearBalls();

	for (ObjectListItc it = rearBalls.begin(); it != rearBalls.end(); it++) {
		ball = *it;
		
		if (closestBall == NULL || ball->distance < closestDistance) {
			closestBall = ball;
			closestDistance = ball->distance;
		}
	}

	return closestBall;
}
