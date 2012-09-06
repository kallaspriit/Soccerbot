#ifndef SOCCERBOT_H
#define SOCCERBOT_H

#include "WebSocketServer.h"

class Robot;
class Serial;

class SoccerBot : public WebSocketServer::ListenerInterface {
    public:
        SoccerBot();
        ~SoccerBot();

        void init();
        void run();

        void onSocketOpen(websocketpp::server::connection_ptr con);
        void onSocketClose(websocketpp::server::connection_ptr con);
        void onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg);

    private:
        Robot* robot;
        WebSocketServer* socket;
        Serial* serial;

        double lastStepTime;
        double lastStepDt;
};

#endif // SOCCERBOT_H
