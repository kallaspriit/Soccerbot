#include "SoccerBot.h"

#include "Serial.h"
#include "Robot.h"
#include "Wheel.h"
#include "WebSocketServer.h"
#include "JsonResponse.h"
#include "Command.h"
#include "Tasks.h"
#include "Util.h"
#include "SignalHandler.h"
#include "Controller.h"
#include "ManualController.h"
#include "TestController.h"
#include "Gui.h"
#include "Camera.h"
#include "Config.h"

SoccerBot::SoccerBot(bool withGui) : lastStepDt(16.666l), withGui(withGui), stopRequested(false) {
    socket = NULL;
    serial = NULL;
    activeController = NULL;
    gui = NULL;
    frontCamera = NULL;
    endCommand = "";
    lastStepTime = Util::millitime();

    originalCoutStream = std::cout.rdbuf();
    stringCoutStream = new std::ostringstream();
    //std::cout.rdbuf(stringCoutStream->rdbuf());
}

SoccerBot::~SoccerBot() {
    std::cout << "! Killing SoccerBot" << std::endl;

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

    if (robot != NULL) {
        std::cout << "! Killing robot.. " << std::endl;

        delete robot;
        robot = NULL;

        std::cout << "! Robot killed!" << std::endl;
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

    for (std::map<std::string, Controller*>::iterator it = controllers.begin(); it != controllers.end(); it++) {
        delete it->second;
    }

    if (gui != NULL) {
        std::cout << "! Killing gui.. ";

        delete gui;
        gui = NULL;

        std::cout << "done!" << std::endl;
    }

    controllers.clear();
    activeController = NULL;
}

void SoccerBot::init() {
    setupFreePort();
    setupSocket();
    setupSerial();
    setupRobot();
    setupControllers();

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

    frontCamera->open(Config::frontCameraSerial);
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

        while (serial->available() > 0) {
            message = serial->read();

            /*std::cout << "Arduino serial: " << message.c_str() << "!" << std::endl;

            socket->broadcast(message);*/
        }

        robot->step(dt);

        //usleep(6000); // some long-running thingy test

        JsonResponse stateResponse("state", getStateJSON());

        socket->broadcast(stateResponse.toJSON());

        lastStepDuration = Util::duration(time);
        lastStepLoad = lastStepDuration * 100.0f / 0.01666f;

        //std::cout << "Last frame time taken: " << lastStepDuration << ", load: " << lastStepLoad << std::endl;

        if (activeController != NULL) {
            activeController->step(dt);
        }

        updateLogs();

        if (gui != NULL) {
            gui->update(dt);
        }

        usleep(16000);
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

    handleRequest(request);

    /*char response[50];
    sprintf(response, "You said: '%s'", msg->get_payload().c_str());
    con->send(response);*/

    //serial->write(msg->get_payload());
}

void SoccerBot::handleRequest(std::string request) {
    if (Command::isValid(request)) {
        Command command = Command::parse(request);

        if (activeController == NULL || !activeController->handleCommand(command)) {
            //std::cout << "! Command: " << command.name << " " << Util::toString(command.params) << std::endl;

            if (command.name == "rebuild") {
                handleRebuildCommand(command);
            } else if (command.name == "set-controller" && command.params.size() == 1) {
                handleSetControllerCommand(command);
            } else {
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
