#ifndef SOCCERBOT_H
#define SOCCERBOT_H

#include "WebSocketServer.h"

#include <map>
#include <string>

class Robot;
class Serial;
class Command;
class SignalHandler;
class Controller;
class Gui;
class FpsCounter;
class Camera;
class Vision;

class SoccerBot : public WebSocketServer::ListenerInterface {
    public:
        SoccerBot(bool withGui = false);
        ~SoccerBot();

        void init();
        void run();

        void setupSignalHandler();
        void setupFreePort();
        void setupSocket();
        void setupSerial();
        void setupRobot();
        void setupControllers();
        void setupGui();
        void setupCameras();
        void setupVision();
        void setupFpsCounter();

        void addController(std::string name, Controller* controller);
        Controller* getController(std::string name);
        bool setController(std::string name);
        std::string getActiveControllerName();

        inline Gui* getGui() const { return gui; }

        void onSocketOpen(websocketpp::server::connection_ptr con);
        void onSocketClose(websocketpp::server::connection_ptr con);
        void onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg);
        void stop() { stopRequested = true; }
        void updateLogs();
        std::string getEndCommand() { return endCommand; }

        void handleRequest(std::string request, websocketpp::server::connection_ptr con);
        void handleRebuildCommand(const Command& cmd);
        void handleGetCameraCalibration(const Command& cmd, websocketpp::server::connection_ptr con);
        void handleGetBlobberCalibration(const Command& cmd, websocketpp::server::connection_ptr con);
        void handleGetFrameCommand(const Command& cmd, websocketpp::server::connection_ptr con);
        void handleSetBlobberCalibration(const Command& cmd);
        void handleSetControllerCommand(const Command& cmd);
        void handleCameraCommand(const Command& cmd);

        std::string getStateJSON() const;

    private:
        Robot* robot;
        WebSocketServer* socket;
        Serial* serial;
        SignalHandler* signalHandler;
        Gui* gui;
        FpsCounter* fpsCounter;
        Camera* frontCamera;
        Vision* vision;

        std::streambuf* originalCoutStream;
        std::ostringstream* stringCoutStream;

        unsigned char* jpegBuffer;
        unsigned char* rgbBuffer;

        double lastStepTime;
        double lastStepDt;
        double lastStepDuration; // @TODO Send to browser
        double lastStepLoad;
        double totalTime;
        bool withGui;
        bool stopRequested;
        std::string endCommand;

        std::map<std::string, Controller*> controllers;
        Controller* activeController;
};

#endif // SOCCERBOT_H
