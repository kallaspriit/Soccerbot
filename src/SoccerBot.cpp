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
#include "Gui.h"
#include "FpsCounter.h"
#include "Camera.h"
#include "Vision.h"
#include "Config.h"

SoccerBot::SoccerBot(bool withGui) : lastStepDt(16.666l), withGui(withGui), stopRequested(false) {
    socket = NULL;
    serial = NULL;
    activeController = NULL;
    gui = NULL;
    fpsCounter = NULL;
    frontCamera = NULL;
    vision = NULL;
    jpegBuffer = NULL;
    rgbBuffer = NULL;
    endCommand = "";
    lastStepTime = Util::millitime();

    originalCoutStream = std::cout.rdbuf();
    stringCoutStream = new std::ostringstream();
    //std::cout.rdbuf(stringCoutStream->rdbuf());
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

        delete stringCoutStream;
        stringCoutStream = NULL;

        std::cout << "done!" << std::endl;
    }
}

void SoccerBot::init() {
    setupSignalHandler();
    setupFreePort();
    setupSocket();
    setupSerial();
    setupRobot();
    setupControllers();
    setupCameras();
    setupVision();
    setupFpsCounter();

    if (withGui) {
        setupGui();
    }

    std::cout << "! SoccerBot ready" << std::endl;
}

void SoccerBot::setupFreePort() {
    int port = Config::socketPort;
    std::string portInfo = Util::exec("lsof -i :" + Util::toString(port) + " | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '");

    if (portInfo.length() > 0) {
        std::cout << "! Killing process on port " << port << " with pid: '" << portInfo << "'.. ";

        Util::exec("kill -2 `lsof -i :" + Util::toString(port) + " | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '`");

        std::cout << "done!" << std::endl << "! Waiting 5 seconds.. ";

        usleep(5000000);

        std::cout << "done!" << std::endl;
    }
}

void SoccerBot::setupSerial() {
    serial = new Serial();

    serial->open("/dev/ttyUSB0", 115200);
}

void SoccerBot::setupSignalHandler() {
    signalHandler = new SignalHandler();

    signalHandler->init();
}

void SoccerBot::setupSocket() {
    socket = new WebSocketServer(Config::socketPort);

    socket->addListener(this);
    socket->start();
}

void SoccerBot::setupRobot() {
    robot = new Robot();

    robot->init();
}

void SoccerBot::setupControllers() {
    addController("manual", new ManualController(robot));
    addController("test", new TestController(robot));
    setController("manual");
}

void SoccerBot::setupGui() {
    gui = new Gui(Config::cameraWidth, Config::cameraHeight);
}

void SoccerBot::setupCameras() {
    frontCamera = new Camera();

    if (!frontCamera->open(Config::frontCameraSerial)) {
        std::cout << "- Failed to find front camera with serial: " << Config::frontCameraSerial << std::endl;

        return;
    }

    frontCamera->setDownsampling(Config::cameraDownsampling);
    frontCamera->setFormat(XI_RAW8);
    frontCamera->setGain(10);
    frontCamera->setExposure(16000);

    std::cout << "! Front camera" << std::endl;
    std::cout << "  > name: " << frontCamera->getName() << std::endl;
    std::cout << "  > type: " << frontCamera->getDeviceType() << std::endl;
    std::cout << "  > api version: " << frontCamera->getApiVersion() << std::endl;
    std::cout << "  > driver version: " << frontCamera->getDriverVersion() << std::endl;
    std::cout << "  > serial number: " << frontCamera->getSerialNumber() << std::endl;
    std::cout << "  > supports color: " << (frontCamera->supportsColor() ? "yes" : "no") << std::endl;
    std::cout << "  > available bandwidth: " << frontCamera->getAvailableBandwidth() << "MB" << std::endl;

    frontCamera->startAcquisition();
}

void SoccerBot::setupVision() {
    vision = new Vision(Config::cameraWidth, Config::cameraHeight);
}

void SoccerBot::setupFpsCounter() {
    fpsCounter = new FpsCounter();
}

void SoccerBot::run() {
    std::string message;
    double time, dt;

    while (!signalHandler->gotExitSignal()/* && totalTime < 5*/ && !stopRequested) {
        time = Util::millitime();
        dt = time - lastStepTime;
        lastStepTime = time;
        lastStepDt = dt;
        totalTime += dt;

        const Camera::FrameYUYV* image = frontCamera->getFrameYUYV();

        vision->onFrameReceived(image->dataYUYV);

        while (serial->available() > 0) {
            message = serial->read();

            /*std::cout << "Arduino serial: " << message.c_str() << "!" << std::endl;

            socket->broadcast(message);*/
        }

        robot->step(dt);

        //usleep(6000); // some long-running thingy test

        JsonResponse stateResponse("state", getStateJSON());

        socket->broadcast(stateResponse.toJSON());

        fpsCounter->step();

        if (fpsCounter->isChanged()) {
            std::cout << "! FPS: " << fpsCounter->getFps() << std::endl;
        }

        updateLogs();

        if (gui != NULL) {
            unsigned char* classification = vision->classify();

            gui->setFrontCameraYUV(image->dataYUYV);
            gui->setFrontCameraClassification(classification);
            gui->update(dt);
        }

        lastStepDuration = Util::duration(time);
        lastStepLoad = lastStepDuration * 100.0f / 0.01666f;

        //std::cout << "Last frame time taken: " << lastStepDuration << ", load: " << lastStepLoad << std::endl;

        if (activeController != NULL) {
            activeController->step(dt);
        }

        //usleep(16000);
    }

    std::cout << "! Shutdown requested" << std::endl;
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
    std::map<std::string, Controller*>::iterator result = controllers.find(name);

    if (result == controllers.end()) {
        return false;
    }

    activeController = result->second;

    return true;
}

std::string SoccerBot::getActiveControllerName() {
    if (activeController == NULL) {
        return "none";
    }

    for (std::map<std::string, Controller*>::iterator it = controllers.begin(); it != controllers.end(); it++) {
        if (it->second == activeController) {
            return it->first;
        }
    }

    return "unknown";
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

        std::string json = "\"" + messages + "\"";

        JsonResponse stateResponse("log", json);

        socket->broadcast(stateResponse.toJSON());
    }

    std::cout.rdbuf(stringCoutStream->rdbuf());
}

void SoccerBot::onSocketOpen(websocketpp::server::connection_ptr con) {
    std::cout << "! Socket connection opened" << std::endl;

    JsonResponse controllerMsg("controller", "\"" + getActiveControllerName() + "\"");

    con->send(controllerMsg.toJSON());
}

void SoccerBot::onSocketClose(websocketpp::server::connection_ptr con) {
    std::cout << "! Socket connection closed" << std::endl;
}

void SoccerBot::onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg) {
    std::string request = msg->get_payload();

    handleRequest(request, con);

    /*char response[50];
    sprintf(response, "You said: '%s'", msg->get_payload().c_str());
    con->send(response);*/

    //serial->write(msg->get_payload());
}

void SoccerBot::handleRequest(std::string request, websocketpp::server::connection_ptr con) {
    if (Command::isValid(request)) {
        Command command = Command::parse(request);

        if (activeController == NULL || !activeController->handleCommand(command)) {
            //std::cout << "! Command: " << command.name << " " << Util::toString(command.params) << std::endl;

            if (command.name == "rebuild") {
                handleRebuildCommand(command);
            } else if (command.name == "set-controller" && command.params.size() == 1) {
                handleSetControllerCommand(command);
            } else if (command.name == "get-camera-calibration") {
                handleGetCameraCalibration(command, con);
            } else if (command.name == "get-blobber-calibration" && command.params.size() == 1) {
                handleGetBlobberCalibration(command, con);
            } else if (command.name == "set-blobber-calibration" && command.params.size() == 7) {
                handleSetBlobberCalibration(command);
            } else if (command.name == "get-frame") {
                handleGetFrameCommand(command, con);
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
    std::string workingDir = Util::getWorkingDirectory();

    endCommand = "bash " + workingDir + "/pull-make-release.sh > build-log.txt";

    stop();
}

void SoccerBot::handleSetControllerCommand(const Command& cmd) {
    std::string controllerName = cmd.params[0];

    if (setController(controllerName)) {
        std::cout << "! Now using '" << controllerName << "' controller" << std::endl;

        JsonResponse controllerMsg("controller", "\"" + getActiveControllerName() + "\"");

        socket->broadcast(controllerMsg.toJSON());
    } else {
        std::cout << "- Unsupported controller '" << controllerName << "' requested" << std::endl;
    }
}

void SoccerBot::handleGetCameraCalibration(const Command& cmd, websocketpp::server::connection_ptr con) {
    Properties cal;
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

    JsonResponse calibrationResponse("blobber-calibration", JSON::stringify(cal));

    con->send(calibrationResponse.toJSON());
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

void SoccerBot::handleSetBlobberCalibration(const Command& cmd) {
    std::string className = cmd.params[0];

    int yLow = Util::toInt(cmd.params[1]);
    int yHigh = Util::toInt(cmd.params[2]);
    int uLow = Util::toInt(cmd.params[3]);
    int uHigh = Util::toInt(cmd.params[4]);
    int vLow = Util::toInt(cmd.params[5]);
    int vHigh = Util::toInt(cmd.params[6]);

    Blobber::Color* color = vision->getBlobber()->getColor(className);

    if (color == NULL) {
        std::cout << "- Requested invalid color: " << className << std::endl;
    }

    color->setThreshold(
        yLow, yHigh,
        uLow, uHigh,
        vLow, vHigh
    );
}

void SoccerBot::handleCameraCommand(const Command& cmd) {
    if (cmd.name == "camera-set-exposure") {
        int value = Util::toInt(cmd.params[0]);

        frontCamera->setExposure(value);
    } else if (cmd.name == "camera-set-gain") {
        float value = Util::toFloat(cmd.params[0]);

        frontCamera->setGain(value);
    } else if (cmd.name == "camera-set-white-balance") {
        float red = Util::toFloat(cmd.params[0]);
        float green = Util::toFloat(cmd.params[1]);
        float blue = Util::toFloat(cmd.params[2]);

        frontCamera->setWhiteBalanceRed(red);
        frontCamera->setWhiteBalanceGreen(green);
        frontCamera->setWhiteBalanceBlue(blue);
    } else if (cmd.name == "camera-set-luminosity-gamma") {
        float value = Util::toFloat(cmd.params[0]);

        frontCamera->setLuminosityGamma(value);
    } else if (cmd.name == "camera-set-chromaticity-gamma") {
        float value = Util::toFloat(cmd.params[0]);

        frontCamera->setChromaticityGamma(value);
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
