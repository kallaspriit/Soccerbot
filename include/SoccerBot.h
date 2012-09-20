#ifndef SOCCERBOT_H
#define SOCCERBOT_H

#include "WebSocketServer.h"

class Robot;
class Serial;
class Command;
class SignalHandler;

class SoccerBot : public WebSocketServer::ListenerInterface {
    public:
        SoccerBot();
        ~SoccerBot();

        void init();
        void run();

        void onSocketOpen(websocketpp::server::connection_ptr con);
        void onSocketClose(websocketpp::server::connection_ptr con);
        void onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg);
        void stop() { stopRequested = true; }
        void updateLogs();
        std::string getEndCommand() { return endCommand; }

        void handleRequest(std::string request);
        void handleTargetVectorCommand(const Command& cmd);
        void handleTargetDirCommand(const Command& cmd);
        void handleRebuildCommand(const Command& cmd);
        void handleResetPositionCommand(const Command& cmd);
        void handleTurnByCommand(const Command& cmd);
        void handleDriveToCommand(const Command& cmd);

        std::string getStateJSON() const;

    private:
        Robot* robot;
        WebSocketServer* socket;
        Serial* serial;
        SignalHandler* signalHandler;

        std::streambuf* originalCoutStream;
        std::ostringstream* stringCoutStream;

        double lastStepTime;
        double lastStepDt;
        double lastStepDuration; // @TODO Send to browser
        double lastStepLoad;
        double totalTime;
        bool stopRequested;
        std::string endCommand;
};

#endif // SOCCERBOT_H
