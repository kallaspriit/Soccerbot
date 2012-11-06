#ifndef CONFIG_H
#define CONFIG_H

#include <Windows.h>
#include "xiApi.h"

#include <string>

namespace Config {

const int socketPort = 8000;
const int dribblerId = 5;
const int coilgunId = 6;
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
const int cameraGain = 5;
const int cameraExposure = 10000;
const int cameraWidth = 1280 / cameraDownsampling;
const int cameraHeight = 1024 / cameraDownsampling;
const int cameraPathStartX = cameraWidth / 2;
const int cameraPathStartY = cameraHeight;
const int frontCameraSerial = 857769553;
const int rearCameraSerial = 857735761;
const std::string blobberConfigFilename = "config/blobber.cfg";

const double kickBackoffTime = 0.5;
const int ballMinArea = 16;
const float validBallSurroundThreshold = 0.45f;
const float validBallPathThreshold = 0.25f; // @TEMP 0.75
const float validGoalPathThreshold = 0.75f;

const float ballFocusK = 8.0f;				//< how fast to yaw to focus on a ball (angle multiplier)
//const float ballChaseK = 1.0f;			//< distance multiplier to chase speed
const float ballFocusMaxOmega = 1.0f;		//< maximum omega to apply while focusing on a ball
//const float ballChaseMaxSpeed = 0.75f;	//< maximum speed to chase a ball at
const float ballCloseThreshold = 0.2f;		//< balls closer than this are considered to be close
const float ballChaseFarSpeed = 1.0f;		//< how fast to chase far-away balls
const float ballChaseNearSpeed = 0.5f;		//< how fast to chase near balls
const float ballCircleSideSpeed = 0.5f;		//< how fast to turn to the side while circling ball
const float ballCircleOmega = 2.0f;			//< how fast to yaw while circling ball
const int dribblerSpeed = 100;				//< normal dribbler operating speed

} // namespace Config

namespace Side {
    const int YELLOW = 1;
    const int BLUE = 2;
}

#endif // CONFIG_H
