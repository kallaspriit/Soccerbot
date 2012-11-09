#include "SoccerBot.h"

#include "Serial.h"
#include "Robot.h"
#include "Wheel.h"
#include "WebSocketServer.h"
#include "JsonResponse.h"
#include "Command.h"
#include "Tasks.h"
#include "Util.h"
#include "JSON.h"
#include "SignalHandler.h"
#include "Controller.h"
#include "ManualController.h"
#include "TestController.h"
#include "BallFollowerController.h"
#include "Gui.h"
#include "FpsCounter.h"
#include "Camera.h"
#include "Dribbler.h"
#include "Vision.h"
#include "Config.h"

SoccerBot::SoccerBot() {
	robot = NULL;
    socket = NULL;
    serial = NULL;
	signalHandler = NULL;
    gui = NULL;
    fpsCounter = NULL;
    frontCamera = NULL;
	rearCamera = NULL;
    vision = NULL;
	activeController = NULL;
	activeControllerName = "";
    jpegBuffer = NULL;
	rgbaBuffer = NULL;
    rgbBuffer = NULL;
    endCommand = "";
	lastStepDt = 0.01666;
	totalTime = 0;
	active = false;
	stopRequested = false;
    lastStepTime = Util::millitime();

    originalCoutStream = std::cout.rdbuf();
    stringCoutStream = new std::ostringstream();
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

    if (serial != NULL) {
        std::cout << "! Killing serial.. ";

        delete serial;
        serial = NULL;

        std::cout << "done!" << std::endl;
    }

    // display latest output and disable special log buffer
    updateLogs();
    std::cout.rdbuf(originalCoutStream);

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
    //setupSerial();
    setupRobot();
    setupCameras();
    setupVision();
	setupControllers();
    setupFpsCounter();
	setupSocket();

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

void SoccerBot::setupSerial() {
	std::cout << "! Opening utility serial.. ";

    serial = new Serial();
	Serial::Result result = serial->open(Config::utilitySerialPort, 115200);

    if (result == Serial::OK) {
		std::cout << "done!" << std::endl;
	} else {
		std::cout << "FAILED!" << std::endl;

		std::cout << "- Failed to open utility serial on port " << Config::utilitySerialPort << " (error: " << result << ")" << std::endl;
	}
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

	Util::sleep(500);
}

void SoccerBot::setupRobot() {
	std::cout << "! Setting up robot.. " << std::endl;

    robot = new Robot();

    robot->init();
}

void SoccerBot::setupGui(HINSTANCE instance) {
	std::cout << "! Setting up GUI.. ";

    gui = new Gui(instance, Config::cameraWidth, Config::cameraHeight);

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

		//delete frontCamera;

		//frontCamera = NULL;
	}

	/*if (rearCamera->open(Config::rearCameraSerial)) {
        configureCamera(rearCamera);
		showCameraInfo(rearCamera, "! Rear camera");

		rearCamera->startAcquisition();
    } else {
		std::cout << "- Failed to find rear camera with serial: " << Config::rearCameraSerial << std::endl;

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

void SoccerBot::setupControllers() {
	std::cout << "! Setting up controllers.. ";

    addController("manual", new ManualController(robot, vision));
    addController("test", new TestController(robot, vision));
    addController("ball-follower", new BallFollowerController(robot, vision));

    setController("manual");

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

		if (updateCameras(dt) == 0) {
			std::cout << "- Failed to get image from either cameras, sleeping for a while.." << std::endl;

			Util::sleep(16);
		}

        if (gui != NULL) {
            gui->update();
        }

        //std::cout << "Seeing " << vision->getBalls().size() << " balls" << std::endl;

        /*while (serial->available() > 0) {
            message = serial->read();

            std::cout << "Utility serial: " << message.c_str() << "!" << std::endl;

			if (socket != NULL) {
				socket->broadcast(message);
			}
			
        }*/

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

        updateLogs();

        lastStepDuration = Util::duration(time);
        lastStepLoad = lastStepDuration * 100.0f / 0.01666f;

        //std::cout << "Last frame time taken: " << lastStepDuration << ", load: " << lastStepLoad << std::endl;

        if (activeController != NULL) {
            activeController->step(dt);
        }

        //usleep(16000);
    }

	active = false;

    std::cout << "! Shutdown requested" << std::endl;
}

int SoccerBot::updateCameras(double dt) {
	ImageBuffer* classification = NULL;
	Camera::FrameYUYV* image = NULL;
	int captures = 0;

	if (frontCamera != NULL && frontCamera->capturing()) {
		image = frontCamera->getFrameYUYV();
		//Util::yuyvToRgb(Config::cameraWidth, Config::cameraHeight, image->dataYUYV, rgbBuffer);
		//gui->setFrontCamera(rgbBuffer);

		if (image != NULL) {
			vision->setFrame(image->dataYUYV);

			if (gui != NULL) {
				classification = vision->classify();
			}

			vision->process(Vision::DIR_FRONT);

			if (gui != NULL) {
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
		activeController = NULL;
		activeControllerName = "";

		return true;
	} else {
		std::map<std::string, Controller*>::iterator result = controllers.find(name);
		
		if (result != controllers.end()) {
			activeController = result->second;
			activeControllerName = name;

			return true;
		} else {
			return false;
		}
    }
}

std::string SoccerBot::getActiveControllerName() {
	return activeControllerName;
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

    //serial->write(msg->get_payload());
	
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
            } else if (command.name == "get-frame") {
                handleGetFrameCommand(command, con);
            } else if (command.name == "stop") {
                handleStopCommand(command);
            } else if (command.name.substr(0, 6) == "camera") {
                handleCameraCommand(command);
            } else if (gui == NULL || !gui->handleCommand(command)) {
                std::cout << "- Unsupported command '" << command.name << "' "<< Util::toString(command.params) << std::endl;
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

void SoccerBot::handleGetFrameCommand(const Command& cmd, websocketpp::server::connection_ptr con) {
    unsigned char* frame = vision->getLastFrame();

    if (frame == NULL) {
        std::cout << "- Unable to send frame, none captured" << std::endl;

        return;
    }

    int jpegBufferSize = 1024 * 100;

    if (jpegBuffer == NULL) {
        jpegBuffer = new unsigned char[jpegBufferSize];
    }

    if (rgbBuffer == NULL) {
        rgbBuffer = new unsigned char[Config::cameraWidth * Config::cameraHeight * 3];
    }

    Util::yuyvToRgb(Config::cameraWidth, Config::cameraHeight, frame, rgbBuffer);

    Util::jpegEncode(rgbBuffer, jpegBuffer, jpegBufferSize, Config::cameraWidth, Config::cameraHeight, 3);
    std::string base64img = Util::base64Encode(jpegBuffer, jpegBufferSize);

    JsonResponse frameResponse("frame", "\"" + base64img + "\"");

    con->send(frameResponse.toJSON());
}

void SoccerBot::handleStopCommand(const Command& cmd) {
	std::cout << "! STOPPING" << std::endl;

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
	stream << "\"fps\":" << fpsCounter->getFps() << ",";

    // wheels
    stream << "\"wheelFL\": {";
    stream << "\"targetOmega\":" << robot->getWheelFL().getTargetOmega() << ",";
    stream << "\"realOmega\":" << robot->getWheelFL().getRealOmega();
    stream << "},";

    // front right wheel
    stream << "\"wheelFR\": {";
    stream << "\"targetOmega\":" << robot->getWheelFR().getTargetOmega() << ",";
    stream << "\"realOmega\":" << robot->getWheelFR().getRealOmega();
    stream << "},";

    // rear left wheel
    stream << "\"wheelRL\": {";
    stream << "\"targetOmega\":" << robot->getWheelRL().getTargetOmega() << ",";
    stream << "\"realOmega\":" << robot->getWheelRL().getRealOmega();
    stream << "},";

    // rear right wheel
    stream << "\"wheelRR\": {";
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
        stream << "\"status\": \"!" << task->toString() << "!\"";
        stream << "}";
    }

    stream << "]";

    stream << "}";

    return stream.str();
}
