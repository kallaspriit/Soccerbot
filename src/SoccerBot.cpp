#include "SoccerBot.h"

#include "Robot.h"
#include "Wheel.h"
#include "WebSocketServer.h"
#include "JsonResponse.h"
#include "Command.h"
#include "Tasks.h"
#include "Util.h"
#include "Maths.h"
#include "JSON.h"
#include "SignalHandler.h"
#include "Controller.h"
#include "ManualController.h"
#include "TestController.h"
#include "SimpleAI.h"
#include "Gui.h"
#include "FpsCounter.h"
#include "Camera.h"
#include "Dribbler.h"
#include "Vision.h"
#include "InfoBoard.h"
#include "DebugRenderer.h"
#include "Config.h"

SoccerBot::SoccerBot() {
	robot = NULL;
    socket = NULL;
	signalHandler = NULL;
    gui = NULL;
    fpsCounter = NULL;
    frontCamera = NULL;
	rearCamera = NULL;
    vision = NULL;
	infoBoard = NULL;
	activeController = NULL;
	activeControllerName = "";
    jpegBuffer = NULL;
	rgbaBuffer = NULL;
    rgbBuffer = NULL;
	classification = NULL;
	stringCoutStream = NULL;
    endCommand = "";
	lastStepDt = 0.01666;
	totalTime = 0;
	cameraChoice = 1;
	targetSide = Side::BLUE;
	go = false;
	active = false;
	stopRequested = false;
	frameRequested = false;
	controllerRequested = false;
	lastFrameRequested = false;
    lastStepTime = Util::millitime();

    //originalCoutStream = std::cout.rdbuf();
    //stringCoutStream = new std::ostringstream();
    //std::cout.rdbuf(stringCoutStream->rdbuf());

	InitializeCriticalSection(&socketMutex);
}

SoccerBot::~SoccerBot() {
    std::cout << "-- Killing SoccerBot --" << std::endl;

    if (gui != NULL) {
        std::cout << "! Killing gui.. ";

        delete gui;
        gui = NULL;

        std::cout << "done!" << std::endl;
    }

    if (fpsCounter != NULL) {
        std::cout << "! Killing fps counter.. ";

        delete fpsCounter;
        fpsCounter = NULL;

        std::cout << "done!" << std::endl;
    }

	if (infoBoard != NULL) {
        std::cout << "! Killing info-board.. ";

        delete infoBoard;
        infoBoard = NULL;

        std::cout << "done!" << std::endl;
    }

    if (vision != NULL) {
        std::cout << "! Killing vision.. ";

        delete vision;
        vision = NULL;

        std::cout << "done!" << std::endl;
    }

    if (frontCamera != NULL) {
        std::cout << "! Killing front camera.. ";

        delete frontCamera;
        frontCamera = NULL;

        std::cout << "done!" << std::endl;
    }

	if (rearCamera != NULL) {
        std::cout << "! Killing rear camera.. ";

        delete rearCamera;
        rearCamera = NULL;

        std::cout << "done!" << std::endl;
    }

    for (std::map<std::string, Controller*>::iterator it = controllers.begin(); it != controllers.end(); it++) {
        delete it->second;
    }

    controllers.clear();
    activeController = NULL;

    if (robot != NULL) {
        std::cout << "! Killing robot.. ";

        delete robot;
        robot = NULL;

        std::cout << "done!" << std::endl;
    }

    // display latest output and disable special log buffer
    //updateLogs();
    //std::cout.rdbuf(originalCoutStream);

    if (socket != NULL) {
        std::cout << "! Killing socket.. ";

        /*delete socket;
        socket = NULL;*/

        socket->close();

        std::cout << "done!" << std::endl;
    }

    if (signalHandler != NULL) {
        std::cout << "! Killing signal handler.. ";

        delete signalHandler;
        signalHandler = NULL;

        std::cout << "done!" << std::endl;
    }

    if (stringCoutStream != NULL) {
        std::cout << "! Killing string-based cout stream.. ";

		std::cout.rdbuf(originalCoutStream);

        delete stringCoutStream;
        stringCoutStream = NULL;

        std::cout << "done!" << std::endl;
    }

	std::cout << "! Removing mutex.. ";
	DeleteCriticalSection(&socketMutex);
	std::cout << "done!" << std::endl;
}

void SoccerBot::init() {
    setupSignalHandler();
    //setupFreePort();
	setupInfoBoard();
    setupCameras();
    setupVision();
	setupRobot();
	setupControllers();
    setupFpsCounter();
	setupSocket();

	setTargetSide(Side::BLUE);

    std::cout << "! SoccerBot ready" << std::endl;
}

void SoccerBot::setupFreePort() {
    /*int port = Config::socketPort;
    std::string portInfo = Util::exec("lsof -i :" + Util::toString(port) + " | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '");

    if (portInfo.length() > 0) {
        std::cout << "! Killing process on port " << port << " with pid: '" << portInfo << "'.. ";

        Util::exec("kill -2 `lsof -i :" + Util::toString(port) + " | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '`");

        std::cout << "done!" << std::endl << "! Waiting 5 seconds.. ";

        usleep(5000000);

        std::cout << "done!" << std::endl;
    }*/
}

void SoccerBot::setupSignalHandler() {
	std::cout << "! Setting up signal handler.. ";

    signalHandler = new SignalHandler();

    signalHandler->init();

	std::cout << "done!" << std::endl;
}

void SoccerBot::setupSocket() {
    socket = new WebSocketServer(Config::socketPort);

    socket->addListener(this);
    socket->listen();
}

void SoccerBot::setupRobot() {
	std::cout << "! Setting up robot.. " << std::endl;

    robot = new Robot(vision);

    if (!robot->init()) {
		std::cout << "- Robot initialization failed!" << std::endl;

		infoBoard->raiseError();
	}
}

void SoccerBot::setupGui(HINSTANCE instance) {
	std::cout << "! Setting up GUI.. ";

    gui = new Gui(instance, Config::cameraWidth, Config::cameraHeight, this);

	std::cout << "done!" << std::endl;
}

void SoccerBot::setupCameras() {
	std::cout << "! Setting up cameras" << std::endl;

    frontCamera = new Camera();
	rearCamera = new Camera();

    if (frontCamera->open(Config::frontCameraSerial)) {
		configureCamera(frontCamera);
		showCameraInfo(frontCamera, "! Front camera");
        
		frontCamera->startAcquisition();
    } else {
		std::cout << "- Failed to find front camera with serial: " << Config::frontCameraSerial << std::endl;

		infoBoard->raiseError();

		//delete frontCamera;

		//frontCamera = NULL;
	}

	/*if (rearCamera->open(Config::rearCameraSerial)) {
        configureCamera(rearCamera);
		showCameraInfo(rearCamera, "! Rear camera");

		rearCamera->startAcquisition();
    } else {
		std::cout << "- Failed to find rear camera with serial: " << Config::rearCameraSerial << std::endl;

		infoBoard->raiseError();

		//delete rearCamera;

		//rearCamera = NULL;
	}*/

	std::cout << "! Cameras ready" << std::endl;
}

void SoccerBot::setupVision() {
	std::cout << "! Setting up vision.. ";

    vision = new Vision(Config::cameraWidth, Config::cameraHeight);

	std::cout << "done!" << std::endl;
}

void SoccerBot::setupInfoBoard() {
	infoBoard = new InfoBoard(Config::infoBoardId);

	infoBoard->setTargetSide(targetSide);
	infoBoard->setGo(go);
	infoBoard->addListener(this);
}

void SoccerBot::setupControllers() {
	std::cout << "! Setting up controllers.. ";

    addController("manual", new ManualController(this));
    addController("test", new TestController(this));
    addController("simple-ai", new SimpleAI(this));

    setController("simple-ai");

	std::cout << "done!" << std::endl;
}

void SoccerBot::setupFpsCounter() {
	std::cout << "! Setting FPS counter.. ";

    fpsCounter = new FpsCounter();

	std::cout << "done!" << std::endl;
}

void SoccerBot::configureCamera(Camera* camera) {
	camera->setDownsampling(Config::cameraDownsampling);
	camera->setFormat(Config::cameraFormat);
	camera->setGain(Config::cameraGain);
	camera->setExposure(Config::cameraExposure);
}

void SoccerBot::showCameraInfo(Camera* camera, std::string name) {
	std::cout << name << std::endl;
	std::cout << "  > name: " << camera->getName() << std::endl;
    std::cout << "  > type: " << camera->getDeviceType() << std::endl;
    std::cout << "  > api version: " << camera->getApiVersion() << std::endl;
    std::cout << "  > driver version: " << camera->getDriverVersion() << std::endl;
    std::cout << "  > serial number: " << camera->getSerialNumber() << std::endl;
    std::cout << "  > supports color: " << (camera->supportsColor() ? "yes" : "no") << std::endl;
    //std::cout << "  > available bandwidth: " << camera->getAvailableBandwidth() << "MB" << std::endl;
}

void SoccerBot::run() {
	std::cout << "! Running the SoccerBot" << std::endl;

    std::string message;
    double time, dt;
    
	rgbaBuffer = new unsigned char[Config::cameraWidth * Config::cameraHeight * 4];
	rgbBuffer = new unsigned char[Config::cameraWidth * Config::cameraHeight * 3];

	active = true;

    while (!stopRequested) {
		if (signalHandler->gotExitSignal()) {
			break;
		}

        time = Util::millitime();
        dt = time - lastStepTime;
        lastStepTime = time;
        lastStepDt = dt;
        totalTime += dt;

		//double s = Util::millitime();
		if (updateCameras(dt) == 0) {
			std::cout << "- Failed to get image from either cameras, sleeping for a while.." << std::endl;

			Util::sleep(16);
		}
		//std::cout << "! Updating cameras took " << Util::duration(s) << " seconds" << std::endl;

        if (gui != NULL) {
			gui->debugParticles(robot->getRobotLocalizer());

            gui->update();
        }

        //std::cout << "Seeing " << vision->getBalls().size() << " balls" << std::endl;

        robot->step(dt);

        //usleep(6000); // some long-running thingy test

		if (socket != NULL) {
			JsonResponse stateResponse("state", getStateJSON());

			socket->broadcast(stateResponse.toJSON());
		}

        fpsCounter->step();

        /*if (fpsCounter->isChanged()) {
            std::cout << "! FPS: " << fpsCounter->getFps() << std::endl;
        }*/

        //updateLogs();

        lastStepDuration = Util::duration(time);
        lastStepLoad = lastStepDuration * 100.0f / 0.01666f;

        //std::cout << "Last frame time taken: " << lastStepDuration << ", load: " << lastStepLoad << std::endl;

        if (activeController != NULL) {
            activeController->step(dt);
        }

		infoBoard->step(dt);

		lastFrameRequested = frameRequested;

		if (frameRequested) {
			sendFrame();

			frameRequested = false;
		}

		if (controllerRequested) {
			JsonResponse controllerMsg("controller", "\"" + getActiveControllerName() + "\"");

			socket->broadcast(controllerMsg.toJSON());

			controllerRequested = false;
		}

		if (targetSide == Side::UNKNOWN) {
			std::cout << "- Target side is unknown" << std::endl;
		}

        //usleep(16000);
    }

	active = false;

    std::cout << "! Shutdown requested" << std::endl;
}

int SoccerBot::updateCameras(double dt) {
	//ImageBuffer* classification = NULL;
	Camera::FrameYUYV* image = NULL;
	int captures = 0;
	//double s;

	if (frontCamera != NULL && frontCamera->capturing()) {
		//s = Util::millitime();
		image = frontCamera->getFrameYUYV();
		//printf("@ Get frame: %.4f\n", Util::duration(s));
		//Util::yuyvToRgb(Config::cameraWidth, Config::cameraHeight, image->dataYUYV, rgbBuffer);
		//gui->setFrontCamera(rgbBuffer);

		if (image != NULL) {
			//s = Util::millitime();
			vision->setFrame(image->dataYUYV);
			//printf("@ Blobber: %.4f\n", Util::duration(s));

			if (gui != NULL || lastFrameRequested) {
				//s = Util::millitime();
				classification = vision->classify();
				//printf("@ Classification: %.4f\n", Util::duration(s));
			}

			//s = Util::millitime();
			vision->process(Vision::DIR_FRONT);
			//printf("@ Process: %.4f\n", Util::duration(s));

			if (gui != NULL) {
				//s = Util::millitime();

				char buf[64];
				sprintf(buf, "FPS: %d", fpsCounter->getFps());
				classification->drawText(10, 10, buf);

				sprintf(buf, "Controller: %s", activeControllerName.c_str());
				classification->drawText(10, 20, buf);

				libyuv::I420ToARGB(
					image->dataY, image->strideY,
					image->dataU, image->strideU,
					image->dataV, image->strideV,
					rgbaBuffer, image->width * 4,
					image->width, image->height
				);
				
				libyuv::ARGBToRGB24(
					rgbaBuffer, image->width * 4,
					rgbBuffer, image->width * 3,
					image->width, image->height
				);

				gui->setFrontCamera(rgbBuffer, classification->data, *vision);

				//printf("@ GUI: %.4f\n", Util::duration(s));
			}

			captures++;
		} else {
			std::cout << "- Failed to get image from the front camera" << std::endl;
		}
	}

	if (rearCamera != NULL && rearCamera->capturing()) {
		image = rearCamera->getFrameYUYV();
		//Util::yuyvToRgb(Config::cameraWidth, Config::cameraHeight, image->dataYUYV, rgbBuffer);
		//gui->setFrontCamera(rgbBuffer);

		if (image != NULL) {
			vision->setFrame(image->dataYUYV);

			if (gui != NULL) {
				classification = vision->classify();
			}

			vision->process(Vision::DIR_REAR);

			if (gui != NULL) {
				libyuv::I420ToARGB(
					image->dataY, image->strideY,
					image->dataU, image->strideU,
					image->dataV, image->strideV,
					rgbaBuffer, image->width * 4,
					image->width, image->height
				);

				libyuv::ARGBToRGB24(
					rgbaBuffer, image->width * 4,
					rgbBuffer, image->width * 3,
					image->width, image->height
				);

				gui->setRearCamera(rgbBuffer, classification->data, *vision);
			}

			captures++;
		} else {
			std::cout << "- Failed to get image from the rear camera" << std::endl;
		}
	}

	return captures;
}

void SoccerBot::addController(std::string name, Controller* controller) {
    controllers[name] = controller;
}

Controller* SoccerBot::getController(std::string name) {
    std::map<std::string, Controller*>::iterator result = controllers.find(name);

    if (result == controllers.end()) {
        return NULL;
    }

    return result->second;
}

bool SoccerBot::setController(std::string name) {
    if (name == "") {
		if (activeController != NULL) {
			activeController->onExit();
		}

		activeController = NULL;
		activeControllerName = "";
		controllerRequested = true;

		return true;
	} else {
		std::map<std::string, Controller*>::iterator result = controllers.find(name);
		
		if (result != controllers.end()) {
			if (activeController != NULL) {
				activeController->onExit();
			}

			activeController = result->second;
			activeControllerName = name;
			activeController->onEnter();

			controllerRequested = true;

			return true;
		} else {
			return false;
		}
    }
}

std::string SoccerBot::getActiveControllerName() {
	return activeControllerName;
}

void SoccerBot::setGo(bool mode) {
	go = mode;

	infoBoard->setGo(mode);

	if (activeController != NULL) {
		activeController->onGoRequestedChange(go);
	}
}

void SoccerBot::setTargetSide(Side side) {
	targetSide = side;

	infoBoard->setTargetSide(side);

	if (activeController != NULL) {
		activeController->onTargetSideChange(side);
	}
}

void SoccerBot::onGoRequestedChange(bool isGoRequested) {
	setGo(isGoRequested);

	if (go) {
		std::cout << "! Go requested" << std::endl;
	} else {
		std::cout << "! Stop requested" << std::endl;
	}
}
void SoccerBot::onTargetSideChange(Side newTargetSide) {
	setTargetSide(newTargetSide);

	if (targetSide == Side::BLUE) {
		std::cout << "! Now targeting blue side" << std::endl;
	} else if (targetSide == Side::YELLOW) {
		std::cout << "! Now targeting yellow side" << std::endl;
	}
}

void SoccerBot::updateLogs() {
    std::cout.rdbuf(originalCoutStream);

    std::string messages = stringCoutStream->str();
    stringCoutStream->clear();
    stringCoutStream->str("");

    if (messages.length() > 0) {
        std::cout << messages;

        bool replaced;

        do {
            replaced = Util::replace(messages, "\n", "\\n");
        } while (replaced);

		if (socket != NULL) {
			std::string json = "\"" + messages + "\"";

			JsonResponse stateResponse("log", json);

			socket->broadcast(stateResponse.toJSON());
		}
    }

    std::cout.rdbuf(stringCoutStream->rdbuf());
}

void SoccerBot::onSocketOpen(websocketpp::server::connection_ptr con) {
	if (!active) {
		return;
	}

	std::cout << "! Opened socket connection" << std::endl;

	controllerRequested = true;

	/*EnterCriticalSection(&socketMutex);
    
    JsonResponse controllerMsg("controller", "\"" + getActiveControllerName() + "\"");

    con->send(controllerMsg.toJSON());
	
	LeaveCriticalSection(&socketMutex);*/
}

void SoccerBot::onSocketClose(websocketpp::server::connection_ptr con) {
	EnterCriticalSection(&socketMutex);
    
	if (!active) {
		return;
	}

    std::cout << "! Socket connection closed" << std::endl;
	
	LeaveCriticalSection(&socketMutex);
}

void SoccerBot::onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg) {
    EnterCriticalSection(&socketMutex);
    
	if (!active) {
		return;
	}
	
	std::string request = msg->get_payload();

    handleRequest(request, con);

    /*char response[50];
    sprintf(response, "You said: '%s'", msg->get_payload().c_str());
    con->send(response);*/

	LeaveCriticalSection(&socketMutex);
}

void SoccerBot::handleRequest(std::string request, websocketpp::server::connection_ptr con) {
    if (Command::isValid(request)) {
        Command command = Command::parse(request);

        if (activeController == NULL || !activeController->handleCommand(command)) {
            //std::cout << "! Command: " << command.name << " " << Util::toString(command.params) << std::endl;

            if (command.name == "rebuild") {
                handleRebuildCommand(command);
            } else if (command.name == "get-controller") {
                handleGetControllerCommand(command, con);
            } else if (command.name == "set-controller" && command.params.size() == 1) {
                handleSetControllerCommand(command);
            } else if (command.name == "get-camera-calibration") {
                handleGetCameraCalibration(command, con);
            } else if (command.name == "get-blobber-calibration" && command.params.size() == 1) {
                handleGetBlobberCalibration(command, con);
            } else if (command.name == "set-blobber-calibration" && command.params.size() == 8) {
                handleSetBlobberCalibration(command);
            } else if (command.name == "blobber-threshold" && command.params.size() == 6) {
                handleBlobberThresholdCommand(command);
            } else if (command.name == "blobber-clear") {
                handleBlobberClearCommand(command);
            } else if (command.name == "get-frame") {
                handleGetFrameCommand(command, con);
            } else if (command.name == "camera-choice") {
                handleCameraChoiceCommand(command);
            } else if (command.name == "stop") {
                handleStopCommand(command);
            } else if (command.name.substr(0, 6) == "camera") {
                handleCameraCommand(command);
            } else if (gui == NULL || !gui->handleCommand(command)) {
                std::cout << "- Unsupported command '" << command.name << "' " << Util::toString(command.params) << std::endl;
            }
        }
    } else {
        if (activeController != NULL) {
            if (!activeController->handleRequest(request)) {
                std::cout << "- Unknown request: " << request << std::endl;
            }
        }
    }
}

void SoccerBot::handleRebuildCommand(const Command& cmd) {
    /*std::string workingDir = Util::getWorkingDirectory();

    endCommand = "bash " + workingDir + "/pull-make-release.sh > build-log.txt";

    stop();*/
}

void SoccerBot::handleGetControllerCommand(const Command& cmd, websocketpp::server::connection_ptr con) {
	/*EnterCriticalSection(&socketMutex);

	if (activeController != NULL) {
		JsonResponse controllerMsg("controller", "\"" + activeControllerName + "\"");

		con->send(controllerMsg.toJSON());
	}

	LeaveCriticalSection(&socketMutex);*/

	/*JsonResponse controllerMsg("controller", "\"" + getActiveControllerName() + "\"");

	if (socket != NULL) {
		socket->broadcast(controllerMsg.toJSON());
	}*/
}

void SoccerBot::handleSetControllerCommand(const Command& cmd) {
    std::string controllerName = cmd.params[0];

    if (setController(controllerName)) {
        std::cout << "! Now using '" << controllerName << "' controller" << std::endl;

        JsonResponse controllerMsg("controller", "\"" + getActiveControllerName() + "\"");

		if (socket != NULL) {
			socket->broadcast(controllerMsg.toJSON());
		}
    } else {
        std::cout << "- Unsupported controller '" << controllerName << "' requested" << std::endl;
    }
}

void SoccerBot::handleGetCameraCalibration(const Command& cmd, websocketpp::server::connection_ptr con) {
    Properties cal;

    cal["exposure"] = Util::toString(frontCamera->getExposure());
    cal["gain"] = Util::toString(frontCamera->getGain());

    JsonResponse calibrationResponse("camera-calibration", JSON::stringify(cal));

    con->send(calibrationResponse.toJSON());
}

void SoccerBot::handleGetBlobberCalibration(const Command& cmd, websocketpp::server::connection_ptr con) {
    std::string className = cmd.params[0];

    Properties cal;
    Blobber::Color* color = vision->getBlobber()->getColor(className);

    if (color == NULL) {
        std::cout << "- Invalid blobber color requested: " << className << std::endl;

        return;
    }

    cal["yLow"] = Util::toString(color->yLow);
    cal["yHigh"] = Util::toString(color->yHigh);
    cal["uLow"] = Util::toString(color->uLow);
    cal["uHigh"] = Util::toString(color->uHigh);
    cal["vLow"] = Util::toString(color->vLow);
    cal["vHigh"] = Util::toString(color->vHigh);
    cal["mergeThreshold"] = Util::toString(color->mergeThreshold);

    JsonResponse calibrationResponse("blobber-calibration", JSON::stringify(cal));

    con->send(calibrationResponse.toJSON());
}

void SoccerBot::handleSetBlobberCalibration(const Command& cmd) {
    std::string className = cmd.params[0];

    int yLow = Util::toInt(cmd.params[1]);
    int yHigh = Util::toInt(cmd.params[2]);
    int uLow = Util::toInt(cmd.params[3]);
    int uHigh = Util::toInt(cmd.params[4]);
    int vLow = Util::toInt(cmd.params[5]);
    int vHigh = Util::toInt(cmd.params[6]);
    double mergeThreshold = (double)Util::toFloat(cmd.params[7]);

    Blobber::Color* color = vision->getBlobber()->getColor(className);

    if (color == NULL) {
        std::cout << "- Requested invalid color: " << className << std::endl;
    }

    color->setThreshold(
        yLow, yHigh,
        uLow, uHigh,
        vLow, vHigh
    );

    //std::cout << "! Set color " << className << " merge threshold: " << mergeThreshold << std::endl;

    color->mergeThreshold = mergeThreshold;
}

void SoccerBot::handleBlobberThresholdCommand(const Command& cmd) {
    std::string className = cmd.params[0];
    int centerX = Util::toInt(cmd.params[1]);
    int centerY = Util::toInt(cmd.params[2]);
    int mode = Util::toInt(cmd.params[3]);
    int brush = Util::toInt(cmd.params[4]);
    float stdev = Util::toInt(cmd.params[5]);
	//int range = 0;

	Camera::YUYV* pixel = NULL;
	Blobber::Color* color = vision->getBlobber()->getColor(className);

	if (color == NULL) {
        std::cout << "- Requested invalid color: " << className << std::endl;

		return;
    }

	int Y, U, V;
	std::vector<float> yValues;
	std::vector<float> uValues;
	std::vector<float> vValues;

	for (int x = -brush; x < brush; x++) {
		int height = (int)Math::sqrt(brush * brush - x * x);

		for (int y = -height; y < height; y++) {
			if (
				x + centerX < 0
				|| x + centerX > Config::cameraWidth - 1
				|| y + centerY < 0
				|| y + centerY > Config::cameraHeight -1
			) {
				continue;
			}

			pixel = frontCamera->getLastFrame()->getPixelAt(x + centerX, y + centerY);

			Y = (pixel->y1 + pixel->y2) / 2;
			U = pixel->u;
			V = pixel->v;

			delete pixel;

			yValues.push_back(Y);
			uValues.push_back(U);
			vValues.push_back(V);
		}
	}

	float yMean, uMean, vMean;
	float yStdDev = Math::standardDeviation(yValues, yMean);
	float uStdDev = Math::standardDeviation(uValues, uMean);
	float vStdDev = Math::standardDeviation(vValues, vMean);

	int minY = yMean - (float)yStdDev * stdev;
	int maxY = yMean + (float)yStdDev * stdev;
	int minU = uMean - (float)uStdDev * stdev;
	int maxU = uMean + (float)uStdDev * stdev;
	int minV = vMean - (float)vStdDev * stdev;
	int maxV = vMean + (float)vStdDev * stdev;
	
	/*int minY = -1, maxY = -1, minU = -1, maxU = -1, minV = -1, maxV = -1;

	for (unsigned int i = 0; i < yValues.size(); i++) {
		Y = yValues.at(i);
		U = uValues.at(i);
		V = vValues.at(i);

		if (
			Y < yMean - yStdDev
			|| Y > yMean + yStdDev
			|| U < uMean - uStdDev
			|| U > uMean + uStdDev
			|| V < vMean - vStdDev
			|| V > vMean + vStdDev
		) {
			continue;
		}

		if (minY == -1 || Y < minY) {
			minY = Y;
		} else if (Y > maxY) {
			maxY = Y;
		}

		if (minU == -1 || U < minU) {
			minU = U;
		} else if (U > maxU) {
			maxU = U;
		}

		if (minV == -1 || V < minV) {
			minV = V;
		} else if (V > maxV) {
			maxV = V;
		}
	}*/

	std::cout << "! Colorpicker [" << mode << "] with brush " << brush << " and stdev " << stdev << std::endl;
	std::cout << "  > Y mean: " << yMean << "; std-dev: " << yStdDev << "; min: " << minY << "; max: " << maxY << std::endl;
	std::cout << "  > U mean: " << uMean << "; std-dev: " << uStdDev << "; min: " << minU << "; max: " << maxU << std::endl;
	std::cout << "  > V mean: " << vMean << "; std-dev: " << vStdDev << "; min: " << minV << "; max: " << maxV << std::endl;

	if (mode == 1) {
		color->setThreshold(
			minY, maxY,
			minU, maxU,
			minV, maxV
		);
	} else if (mode == 2) {
		color->addThreshold(
			minY, maxY,
			minU, maxU,
			minV, maxV
		);
	} else if (mode == 3) {
		color->substractThreshold(
			minY, maxY,
			minU, maxU,
			minV, maxV
		);
	}
}

void SoccerBot::handleBlobberClearCommand(const Command& cmd) {
	if (cmd.params.size() == 1) {
		std::string color = cmd.params[0];

		vision->getBlobber()->clearColor(color);
	} else {
		vision->getBlobber()->clearColors();
	}
}

void SoccerBot::handleGetFrameCommand(const Command& cmd, websocketpp::server::connection_ptr con) {
	frameRequested = true;
}

void SoccerBot::handleCameraChoiceCommand(const Command& cmd) {
	cameraChoice = Util::toInt(cmd.params[0]);

	std::cout << "! Switched camera choice to " << cameraChoice << std::endl;
}

void SoccerBot::handleStopCommand(const Command& cmd) {
	std::cout << "! STOPPING" << std::endl;

	setController("");

    robot->stop();
}

void SoccerBot::handleCameraCommand(const Command& cmd) {
    if (cmd.name == "camera-set-exposure") {
        int value = Util::toInt(cmd.params[0]);

        frontCamera->setExposure(value);
		rearCamera->setExposure(value);
    } else if (cmd.name == "camera-set-gain") {
        float value = Util::toFloat(cmd.params[0]);

        frontCamera->setGain(value);
        rearCamera->setGain(value);
    } else if (cmd.name == "camera-set-white-balance") {
        float red = Util::toFloat(cmd.params[0]);
        float green = Util::toFloat(cmd.params[1]);
        float blue = Util::toFloat(cmd.params[2]);

		std::cout << "! Set white balance: " << red << ", " << green << ", " << blue << std::endl;

        frontCamera->setWhiteBalanceRed(red);
        frontCamera->setWhiteBalanceGreen(green);
        frontCamera->setWhiteBalanceBlue(blue);

		rearCamera->setWhiteBalanceRed(red);
        rearCamera->setWhiteBalanceGreen(green);
        rearCamera->setWhiteBalanceBlue(blue);
    } else if (cmd.name == "camera-set-luminosity-gamma") {
        float value = Util::toFloat(cmd.params[0]);

        frontCamera->setLuminosityGamma(value);
        rearCamera->setLuminosityGamma(value);
    } else if (cmd.name == "camera-set-chromaticity-gamma") {
        float value = Util::toFloat(cmd.params[0]);

        frontCamera->setChromaticityGamma(value);
        rearCamera->setChromaticityGamma(value);
    }
}

void SoccerBot::sendFrame() {
	// @TODO Fetch frame based on cameraChoice
	unsigned char* frame = vision->getLastFrame();

    if (frame == NULL) {
        std::cout << "- Unable to send frame, none captured" << std::endl;

        return;
    }

    int jpegBufferSize = 1024 * 1000;

    if (jpegBuffer == NULL) {
        jpegBuffer = new unsigned char[jpegBufferSize];
    }

    if (rgbBuffer == NULL) {
        rgbBuffer = new unsigned char[Config::cameraWidth * Config::cameraHeight * 3];
    }

    Util::yuyvToRgb(Config::cameraWidth, Config::cameraHeight, frame, rgbBuffer);
	DebugRenderer::render(rgbBuffer, vision->getFrontBalls(), vision->getFrontGoals());

    Util::jpegEncode(rgbBuffer, jpegBuffer, jpegBufferSize, Config::cameraWidth, Config::cameraHeight, 3);
    std::string base64Rgb = Util::base64Encode(jpegBuffer, jpegBufferSize);

	jpegBufferSize = 1024 * 1000;

	if (classification == NULL) {
		classification = vision->classify();
	}

	vision->setImage(classification->data);

	Util::jpegEncode(classification->data, jpegBuffer, jpegBufferSize, Config::cameraWidth, Config::cameraHeight, 3);
	std::string base64Classification = Util::base64Encode(jpegBuffer, jpegBufferSize);

	JsonResponse frameResponse("frame", "{\"rgb\": \"" + base64Rgb + "\",\"classification\": \"" + base64Classification + "\"}");

	socket->broadcast(frameResponse.toJSON());
}

std::string SoccerBot::getStateJSON() const {
    Math::Position pos = robot->getPosition();

    std::stringstream stream;

    stream << "{";

    // general robot info
    stream << "\"x\":" << pos.x << ",";
    stream << "\"y\":" << pos.y << ",";
    stream << "\"orientation\":" << pos.orientation << ",";
    stream << "\"dt\":" << lastStepDt << ",";
    stream << "\"load\":" << lastStepLoad << ",";
    stream << "\"duration\":" << lastStepDuration << ",";
    stream << "\"totalTime\":" << totalTime << ",";
	stream << "\"gotBall\":" << robot->getDribbler().gotBall() << ",";

    // wheels
    stream << "\"wheelFL\": {";
	stream << "\"stalled\":" << (robot->getWheelFL().isStalled() ? "true" : "false") << ",";
    stream << "\"targetOmega\":" << robot->getWheelFL().getTargetOmega() << ",";
    stream << "\"realOmega\":" << robot->getWheelFL().getRealOmega();
    stream << "},";

    // front right wheel
    stream << "\"wheelFR\": {";
	stream << "\"stalled\":" << (robot->getWheelFR().isStalled() ? "true" : "false") << ",";
    stream << "\"targetOmega\":" << robot->getWheelFR().getTargetOmega() << ",";
    stream << "\"realOmega\":" << robot->getWheelFR().getRealOmega();
    stream << "},";

    // rear left wheel
    stream << "\"wheelRL\": {";
	stream << "\"stalled\":" << (robot->getWheelRL().isStalled() ? "true" : "false") << ",";
    stream << "\"targetOmega\":" << robot->getWheelRL().getTargetOmega() << ",";
    stream << "\"realOmega\":" << robot->getWheelRL().getRealOmega();
    stream << "},";

    // rear right wheel
    stream << "\"wheelRR\": {";
	stream << "\"stalled\":" << (robot->getWheelRR().isStalled() ? "true" : "false") << ",";
    stream << "\"targetOmega\":" << robot->getWheelRR().getTargetOmega() << ",";
    stream << "\"realOmega\":" << robot->getWheelRR().getRealOmega();
    stream << "},";

    // tasks
    stream << "\"tasks\": [";

    TaskQueue tasks = robot->getTasks();
    bool first = true;

    for (TaskQueueIt it = tasks.begin(); it != tasks.end(); it++) {
        Task* task = *it;

        if (!first) {
            stream << ",";
        } else {
            first = false;
        }

        stream << "{";
        stream << "\"started\": " << (task->isStarted() ? "true" : "false") << ",";
        stream << "\"percentage\": " << task->getPercentage() << ",";
        stream << "\"status\": \"" << task->toString() << "\"";
        stream << "}";
    }

    stream << "],";

	// balls
	ObjectList* balls = NULL;
	ObjectList frontBalls = vision->getFrontBalls();
	ObjectList rearBalls = vision->getRearBalls();
	Object* ball;

	first = true;

	stream << "\"balls\": [";

	for (int i = 0; i < 2; i++) {
		balls = i == 0 ? &frontBalls : &rearBalls;

		for (ObjectListItc it = balls->begin(); it != balls->end(); it++) {
			ball = *it;

			if (!first) {
				stream << ",";
			} else {
				first = false;
			}

			stream << "{";
			stream << "\"x\": " << ball->x << ",";
			stream << "\"y\": " << ball->y << ",";
			stream << "\"width\": " << ball->width << ",";
			stream << "\"height\": " << ball->height << ",";
			stream << "\"distance\": " << ball->distance << ",";
			stream << "\"angle\": " << ball->angle << ",";
			stream << "\"camera\": \"" << (i == 0 ? "front" : "rear") << "\"";
			stream << "}";
		}
	}

	stream << "],";

	// goals
	ObjectList* goals = NULL;
	ObjectList frontGoals = vision->getFrontGoals();
	ObjectList rearGoals = vision->getRearGoals();
	Object* goal;

	first = true;

	stream << "\"goals\": [";

	for (int i = 0; i < 2; i++) {
		goals = i == 0 ? &frontGoals : &rearGoals;
		
		for (ObjectListItc it = goals->begin(); it != goals->end(); it++) {
			goal = *it;

			if (!first) {
				stream << ",";
			} else {
				first = false;
			}

			stream << "{";
			stream << "\"x\": " << goal->x << ",";
			stream << "\"y\": " << goal->y << ",";
			stream << "\"width\": " << goal->width << ",";
			stream << "\"height\": " << goal->height << ",";
			stream << "\"distance\": " << goal->distance << ",";
			stream << "\"angle\": " << goal->angle << ",";
			stream << "\"camera\": \"" << (i == 0 ? "front" : "rear") << "\"";
			stream << "}";
		}
	}

	stream << "],";

	stream << "\"measurements\": [";

	const Measurements measurements = robot->getMeasurements();

	first = true;

	for (Measurements::const_iterator it = measurements.begin(); it != measurements.end(); it++) {
		if (!first) {
			stream << ",";
		} else {
			first = false;
		}

		stream << "\"" + it->first + "\": \"" + Util::toString(it->second) + "\"";
	}

	stream << "],";

	if (activeController != NULL) {
		stream << "\"controllerName\": \"" + activeControllerName + "\",";
		std::string controllerInfo = activeController->getJSON();

		if (controllerInfo.length() > 0) {
			stream << "\"controllerState\": " << controllerInfo << ",";
		} else {
			stream << "\"controllerState\": null,";
		}
	} else {
		stream << "\"controllerName\": null,";
		stream << "\"controllerState\": null,";
	}

	stream << "\"fps\":" << fpsCounter->getFps()  << ",";
	stream << "\"targetGoal\":" << targetSide << ",";
	stream << "\"isError\":" << (infoBoard->isError() ? "1" : "0") << ",";
	stream << "\"isGo\":" << (go ? "1" : "0");

    stream << "}";

    return stream.str();
}
