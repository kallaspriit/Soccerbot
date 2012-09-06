#include "SoccerBot.h"

#include "serial/Serial.h"

#include "Robot.h"
#include "WebSocketServer.h"
#include "JsonResponse.h"
#include "Util.h"

SoccerBot::SoccerBot() : lastStepDt(16.666l) {
    socket = NULL;
    serial = NULL;
    lastStepTime = Util::now();
}

SoccerBot::~SoccerBot() {
    if (serial != NULL) {
        delete serial;
        serial = NULL;
    }

    if (socket != NULL) {
        delete socket;
        socket = NULL;
    }

    if (robot != NULL) {
        delete robot;
        robot = NULL;
    }
}

void SoccerBot::init() {
    robot = new Robot();
    socket = new WebSocketServer(8000);
    serial = new Serial();

    serial->open("/dev/ttyUSB0", 115200);

    socket->addListener(this);
    socket->start();

    robot->init();

    std::cout << "! SoccerBot ready" << std::endl;
}

void SoccerBot::run() {
    std::string message;
    double time, dt;

    while (true) {
        time = Util::now();
        dt = time - lastStepTime;
        lastStepTime = time;
        lastStepDt = dt;

        while (serial->available() > 0) {
            message = serial->read();

            std::cout << "Arduino serial: " << message.c_str() << "!" << std::endl;

            socket->broadcast(message);
        }

        robot->step(dt);

        JsonResponse stateResponse("state", robot->getStateJSON());

        socket->broadcast(stateResponse.toJSON());

        usleep(16000);
    }
}

void SoccerBot::onSocketOpen(websocketpp::server::connection_ptr con) {
    std::cout << "! Socket connection opened" << std::endl;
}

void SoccerBot::onSocketClose(websocketpp::server::connection_ptr con) {
    std::cout << "! Socket connection closed" << std::endl;
}

void SoccerBot::onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg) {
    std::cout << "! Message received: " << msg->get_payload() << std::endl;

    /*char response[50];
    sprintf(response, "You said: '%s'", msg->get_payload().c_str());
    con->send(response);*/

    serial->write(msg->get_payload());
}
