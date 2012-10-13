#ifndef CONFIG_H
#define CONFIG_H

namespace Config {

const int socketPort = 8000;
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
const int cameraWidth = 1280;
const int cameraHeight = 1024;
const int frontCameraSerial = 0;

} // namespace Config

#endif // CONFIG_H
