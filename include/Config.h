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
const float robotRadius = 0.125f;
const float ballRadius = 0.021335f;
const float ballElasticity = 0.3f;
const float ballDrag = 0.2f;

const int ballLocalizerParticleCount = 1000;
const float ballLocalizerForwardNoise = 0.8f;
const float ballLocalizerTurnNoise = 0.3f;
const float ballLocalizerSenseNoise = 0.5f;
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
const float cameraCorrectionK = 0.00000049f;
const float cameraCorrectionZoom = 0.969f;
const int cameraPathStartX = cameraWidth / 2;
const int cameraPathStartY = cameraHeight - 80;
const int frontCameraSerial = 857769553;
const int rearCameraSerial = 857735761;
const std::string blobberConfigFilename = "config/blobber.cfg";

const int viewObstructedX = 100;
const int viewObstructedY = 100;
const int viewObstructedWidth = cameraWidth - viewObstructedX * 2;
const int viewObstructedHeight = cameraHeight - viewObstructedY * 2;

const int robotInWayWidth = 120;
const int robotInWayHeight = 150;
const int robotInWayY = 60;

const float viewObstructedThreshold = 0.5f;
const float ballInGoalThreshold = 0.25f;
const float fakeObjectLifetime = 1.0f;
const float fakeBallLifetime = 0.1f;
const int wheelStalledThreshold = 60;

const double kickBackoffTime = 1.0;
const int ballMinArea = 4;
//const int ballMaxInvalidSpree = 30;					//< max number of invalid color steps in a row for a valid ball
//const int goalMaxInvalidSpree = 3;					//< max number of invalid color steps in a row for a valid ball
const float ballInvalidSpreeScaler = 0.55f;
const int ballMinInvalidSpree = 4;
const float goalInvalidSpreeScaler = 0.55f;
const int goalMinInvalidSpree = 1;
const float validBallSurroundThreshold = 0.5f;
const float validBallPathThreshold = 0.65f;			//< which percentage of pixels need to be valid on ball path
const float validGoalPathThreshold = 0.75f;			//< which percentage of pixels need to be valid on goal path
const float blobberPickerStdDevMultiplier = 1.5f;
const int undersideMetricBaseSteps = 20;
const double ballInDribblerThreshold = 0.25f;		//< how long has the ball be in dribbler to be considered to have ball
const double dribblerBallLostThreshold = 0.25f;		//< how long must the ball have left the dribbler to consider have lost the ball

const float fluidSpeedStep = 3.0f;
const float fluidOmegaStep = 9.42f;

const float ballFocusP = 5.0f;						//< how fast to yaw to focus on a ball (angle multiplier)
const float ballFocusI = 0.0f;						//< how fast to yaw to focus on a ball (angle multiplier)
const float ballFocusD = 0.0f;						//< how fast to yaw to focus on a ball (angle multiplier)

const float goalFocusP = ballFocusP;				//< how fast to yaw to focus on a goal (angle multiplier)
const float ballSearchOmega = 3.14f;				//< how fast to spin to search for ball
const float ballRotateOmega = 2.0f;					//< how fast to rotate while searching for goal
const float goalSpinOmega = 1.5f;					//< how fast to rotate while searching for goal
//const float ballChaseK = 1.0f;					//< distance multiplier to chase speed
const float focusMaxOmega = ballSearchOmega;		//< maximum omega to apply while focusing on an object
//const float ballChaseMaxSpeed = 0.75f;			//< maximum speed to chase a ball at
const float ballCloseThreshold = 0.27f;				//< balls closer than this are considered to be close
const float dribblerOnThreshold = 0.2f;				//< distance at which to turn on the dribbler
const float ballChaseFarSpeed = 2.4f;				//< how fast to chase far-away balls
const float ballChaseNearSpeed = 0.45f;				//< how fast to chase near balls
const float goalAimSpeed = 0.35f;					//< how fast to move forward while aiming
//const float ballSlowDownAngleThreshold = 20.0f;	//< if the ball angle is larger than this then drive slower
//const float ballCircleSideSpeed = 0.5f;			//< how fast to turn to the side while circling ball
//const float ballCircleOmega = 2.0f;				//< how fast to yaw while circling ball
const int dribblerSpeed = 100;						//< normal dribbler operating speed
const float spinAroundDribblerPeriod = 3.0f;		//< in how many seconds to spin around the dribbler
const float spinAroundDribblerRadius = 0.1f;		//< radius at which to spin around the dribbler
const float spinAroundDribblerForwardSpeed = 0.3f;	//< radius at which to spin around the dribbler
const float goalMinUndersideMetric = 0.50f;			//< minimum amount of valid pixels under goal
const int goalMinArea = 64;
const int goalTopMaxY = 10;
const int validGoalMinMatches = 10;
const int whiteBlackMinY = 100;
const int maxBallSenseRadius = 100;
const float rotationStoppedOmegaThreshold = 0.25f;
const float rotationCancelMultiplier = 0.3f;
const float ballChaseAngleSlowdownMultiplier = 4.0f;
const float ballChaseAngleMaxSlowdown = 0.8f;
//const float chaseBallBrakeMultiplier = 20.0f;
const float chaseBallBrakeMultiplier = 1.25f;
const float chaseBallMaxBrakeSpeed = 2.0f;
const float brakeDistanceMultiplier = 1.25f;
//const float distanceCorrection = -0.03f;
//const float distanceCorrection = -0.13f;
const float distanceCorrection = 0.0f;
const float chaseDistanceCorrection = -0.18f;
const int maxSurroundSenseY = cameraHeight - 50;
const float jumpAngleStopMultiplier = 1.0f;
const float goalKickThreshold = 0.35f;
const int wideGoalThreshold = 100;
const float kickBallAvoidSideSpeed = 0.3f;

} // namespace Config

enum Side {
    YELLOW = 1,
    BLUE = 2,
	UNKNOWN = 3
};

#endif // CONFIG_H
