#include "SoccerBot.h"

#include "Serial.h"
#include "Robot.h"
#include "WebSocketServer.h"
#include "JsonResponse.h"
#include "Command.h"
#include "Util.h"
#include "SignalHandler.h"

SoccerBot::SoccerBot() : lastStepDt(16.666l) {
    socket = NULL;
    serial = NULL;
    lastStepTime = Util::now();
}

SoccerBot::~SoccerBot() {
    std::cout << "! Killing SoccerBot" << std::endl;

    if (serial != NULL) {
        std::cout << "! Killing serial.. ";

        delete serial;
        serial = NULL;

        std::cout << "done!" << std::endl;
    }

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
}

void SoccerBot::init() {
    signalHandler = new SignalHandler();
    robot = new Robot();
    socket = new WebSocketServer(8000);
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

    while (!signalHandler->gotExitSignal()/* && totalTime < 5*/) {
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

        JsonResponse stateResponse("state", robot->getStateJSON());

        socket->broadcast(stateResponse.toJSON());

        lastStepDuration = Util::duration(time);
        lastStepLoad = lastStepDuration * 100.0f / 16.6666f;

        //std::cout << "Last frame time taken: " << lastStepDuration << ", load: " << lastStepLoad << std::endl;

        usleep(16000);
    }

    std::cout << "! Shutdown requested" << std::endl;
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

        //std::cout << "Command: " << command.name << " " << Util::toString(command.params) << std::endl;

        if (command.name == "target-vector" && command.params.size() == 3) {
            handleTargetVectorCommand(command);
        } else if (command.name == "target-dir" && command.params.size() == 3) {
            handleTargetDirCommand(command);
        } else if (command.name == "rebuild") {
            handleRebuildCommand(command);
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

    //std::cout << "Set robot dir by vector: " << x << "x" << y << " with omega " << omega << std::endl;
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
    std::string command = "bash " + workingDir + "/pull-make-release.sh > build-log.txt";

    std::string result = Util::exec(command);

    std::cout << "Rebuild command: '" << command << "' result: " << result << std::endl;

    exit(0);
}
