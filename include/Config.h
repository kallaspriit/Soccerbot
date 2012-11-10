#ifndef CONFIG_H
#define CONFIG_H

//#include "Maths.h"

#include <Windows.h>
#include "xiApi.h"

#include <string>

namespace Config {

const int socketPort = 8000;
const int dribblerId = 5;
const int coilgunId = 6;
const int infoBoardId = 7;
const float fieldWidth = 4.5f;
const float fieldHeight = 3.0f;
const float ballRadius = 0.021335f;
const float ballElasticity = 0.3f;
const float ballDrag = 0.2f;

const float ballLocalizerVelocityUpdateMaxTime = 0.025f;
const float maxBallIdentityDistance = 0.25f;
const float ballRemoveTime = 0.3f;
const float ballPurgeLifetime = 10.0f;
const float ballMaxVelocity = 8.0f;
const float ballFovCloseEnough = 0.5f;

const std::string utilitySerialPort = "COM1";

const int cameraDownsampling = 2;
const int cameraFormat = XI_RAW8;
const int cameraGain = 6;
const int cameraExposure = 10000;
const int cameraWidth = 1280 / cameraDownsampling;
const int cameraHeight = 1024 / cameraDownsampling;
const int cameraPathStartX = cameraWidth / 2;
const int cameraPathStartY = cameraHeight - 80;
const int frontCameraSerial = 857769553;
const int rearCameraSerial = 857735761;
const std::string blobberConfigFilename = "config/blobber.cfg";

const double kickBackoffTime = 0.5;
const int ballMinArea = 16;
//const int ballMaxInvalidSpree = 30;					//< max number of invalid color steps in a row for a valid ball
//const int goalMaxInvalidSpree = 3;					//< max number of invalid color steps in a row for a valid ball
const float ballInvalidSpreeScaler = 0.55f;
const int ballMinInvalidSpree = 4;
const float goalInvalidSpreeScaler = 0.55f;
const int goalMinInvalidSpree = 1;
const float validBallSurroundThreshold = 0.5f;
const float validBallPathThreshold = 0.65f;			//< which percentage of pixels need to be valid on ball path
const float validGoalPathThreshold = 0.75f;			//< which percentage of pixels need to be valid on goal path

const float ballFocusP = 5.0f;						//< how fast to yaw to focus on a ball (angle multiplier)
const float ballFocusI = 0.0f;						//< how fast to yaw to focus on a ball (angle multiplier)
const float ballFocusD = 0.0f;						//< how fast to yaw to focus on a ball (angle multiplier)

const float goalFocusP = ballFocusP;				//< how fast to yaw to focus on a goal (angle multiplier)
//const float ballChaseK = 1.0f;					//< distance multiplier to chase speed
const float ballFocusMaxOmega = 6.28f;				//< maximum omega to apply while focusing on a ball
const float goalFocusMaxOmega = ballFocusMaxOmega;	//< maximum omega to apply while focusing on a ball
//const float ballChaseMaxSpeed = 0.75f;			//< maximum speed to chase a ball at
const float ballCloseThreshold = 0.25f;				//< balls closer than this are considered to be close
const float dribblerOnThreshold = 0.3f;				//< distance at which to turn on the dribbler
const float ballChaseFarSpeed = 1.7f;				//< how fast to chase far-away balls
const float ballChaseNearSpeed = 0.45f;				//< how fast to chase near balls
//const float ballSlowDownAngleThreshold = 20.0f;	//< if the ball angle is larger than this then drive slower
//const float ballCircleSideSpeed = 0.5f;			//< how fast to turn to the side while circling ball
//const float ballCircleOmega = 2.0f;				//< how fast to yaw while circling ball
const int dribblerSpeed = 100;						//< normal dribbler operating speed
const float spinAroundDribblerPeriod = 2.0f;		//< in how many seconds to spin around the dribbler
const float spinAroundDribblerRadius = 0.15f;		//< radius at which to spin around the dribbler
const float spinAroundDribblerForwardSpeed = 0.5f;	//< radius at which to spin around the dribbler
const float goalMinUndersideMetric = 0.70f;			//< minimum amount of valid pixels under goal
const int validGoalMinMatches = 10;
const int whiteBlackMinY = 100;
const int maxBallSenseRadius = 100;
const float rotationStoppedOmegaThreshold = 0.25f;
const float rotationCancelMultiplier = 0.3f;
const float ballChaseAngleSlowdownMultiplier = 4.0f;
const float ballChaseAngleMaxSlowdown = 0.8f;
//const float chaseBallBrakeMultiplier = 20.0f;
const float chaseBallBrakeMultiplier = 1.0f;
const float chaseBallMaxBrakeSpeed = 2.0f;
const float brakeDistanceMultiplier = 1.0f;
const float distanceCorrection = -0.03f;
const int maxSurroundSenseY = cameraHeight / 2 - 50;
const float jumpAngleStopMultiplier = 1.0f;

} // namespace Config

enum Side {
    YELLOW = 1,
    BLUE = 2,
	UNKNOWN = 3
};

#endif // CONFIG_H
