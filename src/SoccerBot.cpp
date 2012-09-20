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

SoccerBot::SoccerBot() : lastStepDt(16.666l), stopRequested(false) {
    socket = NULL;
    serial = NULL;
    endCommand = "";
    lastStepTime = Util::now();

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
}

void SoccerBot::init() {
    int port = 8000;
    std::string portInfo = Util::exec("lsof -i :" + Util::toString(port) + " | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '");

    if (portInfo.length() > 0) {
        std::cout << "! Killing process on port " << port << " with pid: '" << portInfo << "'.. ";

        Util::exec("kill -2 `lsof -i :" + Util::toString(port) + " | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '`");

        std::cout << "done!" << std::endl << "! Waiting 5 seconds.. ";

        usleep(5000000);

        std::cout << "done!" << std::endl;
    }

    signalHandler = new SignalHandler();
    robot = new Robot();
    socket = new WebSocketServer(port);
    serial = new Serial();

    serial->open("/dev/ttyUSB0", 115200);

    socket->addListener(this);
    socket->start();

    robot->init();
    signalHandler->init();

    std::cout << "! SoccerBot ready" << std::endl;
}

void SoccerBot::run() {
    std::string message;
    double time, dt;

    while (!signalHandler->gotExitSignal()/* && totalTime < 5*/ && !stopRequested) {
        time = Util::now();
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

        updateLogs();

        usleep(16000);
    }

    std::cout << "! Shutdown requested" << std::endl;
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

        //std::cout << "! Command: " << command.name << " " << Util::toString(command.params) << std::endl;

        if (command.name == "target-vector" && command.params.size() == 3) {
            handleTargetVectorCommand(command);
        } else if (command.name == "target-dir" && command.params.size() == 3) {
            handleTargetDirCommand(command);
        } else if (command.name == "rebuild") {
            handleRebuildCommand(command);
        } else if (command.name == "reset-position") {
            handleResetPositionCommand(command);
        } else if (command.name == "turn-by" && command.params.size() == 2) {
            handleTurnByCommand(command);
        } else if (command.name == "drive-to" && command.params.size() == 4) {
            handleDriveToCommand(command);
        } else {
            std::cout << "- Unsupported command '" << command.name << "' "<< Util::toString(command.params) << std::endl;
        }
    } else {
        std::cout << "- Not a command: " << request << std::endl;
    }
}

void SoccerBot::handleTargetVectorCommand(const Command& cmd) {
    float x = Util::toFloat(cmd.params[0]);
    float y = Util::toFloat(cmd.params[1]);
    float omega = Util::toFloat(cmd.params[2]);

    robot->setTargetDir(x, y, omega);

    //std::cout << "! Set robot dir by vector: " << x << "x" << y << " with omega " << omega << std::endl;
}

void SoccerBot::handleTargetDirCommand(const Command& cmd) {
    Math::Deg dir = Math::Deg(Util::toFloat(cmd.params[0]));

    float speed = Util::toFloat(cmd.params[1]);
    float omega = Util::toFloat(cmd.params[2]);

    robot->setTargetDir(dir, speed, omega);

    //std::cout << "Set robot dir by orientation: " << dir.deg() << ", speed: " << speed << " and omega " << omega << std::endl;
}

void SoccerBot::handleRebuildCommand(const Command& cmd) {
    std::string workingDir = Util::getWorkingDirectory();

    endCommand = "bash " + workingDir + "/pull-make-release.sh > build-log.txt";

    stop();
}

void SoccerBot::handleResetPositionCommand(const Command& cmd) {
    robot->setPosition(0.125f, 0.125f, 0);
}

void SoccerBot::handleTurnByCommand(const Command& cmd) {
    float angle = Util::toFloat(cmd.params[0]);
    float speed = Util::toFloat(cmd.params[1]);

    robot->turnBy(angle, speed);
}

void SoccerBot::handleDriveToCommand(const Command& cmd) {
    float x = Util::toFloat(cmd.params[0]);
    float y = Util::toFloat(cmd.params[1]);
    float orientation = Util::toFloat(cmd.params[2]);
    float speed = Util::toFloat(cmd.params[3]);

    robot->driveTo(x, y, orientation, speed);
}

std::string SoccerBot::getStateJSON() const {
    Math::Vector pos = robot->getPosition();

    std::stringstream stream;

    stream << "{";

    // general robot info
    stream << "\"x\":" << pos.x << ",";
    stream << "\"y\":" << pos.y << ",";
    stream << "\"orientation\":" << robot->getOrientation() << ",";
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

        std::string desc = task->toString();

        if (!first) {
            stream << ",";
        } else {
            first = false;
        }

        stream << "\"" << desc << "\"";

        /*stream << "{";
        stream << "\"status\": \"" << desc << "\"";
        stream << "}";*/
    }

    stream << "]";


    stream << "}";

    return stream.str();
}
