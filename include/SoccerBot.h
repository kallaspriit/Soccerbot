#ifndef SOCCERBOT_H
#define SOCCERBOT_H

#include "WebSocketServer.h"

#include <map>
#include <string>

class Robot;
class Command;
class SignalHandler;
class Controller;
class Gui;
class FpsCounter;
class Camera;
class Vision;
class InfoBoard;
class ImageBuffer;

class SoccerBot : public WebSocketServer::ListenerInterface {
    public:
        SoccerBot();
        ~SoccerBot();

        void init();
        void run();

        void setupSignalHandler();
        void setupFreePort();
        void setupSocket();
        void setupRobot();
        void setupGui(HINSTANCE instance);
        void setupCameras();
        void setupVision();
		void setupInfoBoard();
		void setupControllers();
        void setupFpsCounter();

        void addController(std::string name, Controller* controller);
        Controller* getController(std::string name);
        bool setController(std::string name);
        std::string getActiveControllerName();

        inline Gui* getGui() const { return gui; }
		inline Vision* getVision() const { return vision; }
		inline Camera* getFrontCamera() const { return frontCamera; }

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
        void handleStopCommand(const Command& cmd);
        void handleSetBlobberCalibration(const Command& cmd);
		void handleGetControllerCommand(const Command& cmd, websocketpp::server::connection_ptr con);
        void handleSetControllerCommand(const Command& cmd);
        void handleCameraCommand(const Command& cmd);
		void handleBlobberThresholdCommand(const Command& cmd);
		void handleBlobberClearCommand(const Command& cmd);

        std::string getStateJSON() const;

    private:
		void configureCamera(Camera* camera);
		void showCameraInfo(Camera* camera, std::string name);
		void sendFrame();
		int updateCameras(double dt);

        Robot* robot;
        WebSocketServer* socket;
        SignalHandler* signalHandler;
        Gui* gui;
        FpsCounter* fpsCounter;
        Camera* frontCamera;
		Camera* rearCamera;
        Vision* vision;
		InfoBoard* infoBoard;

        std::streambuf* originalCoutStream;
        std::ostringstream* stringCoutStream;

        unsigned char* jpegBuffer;
		unsigned char* rgbaBuffer;
        unsigned char* rgbBuffer;

        double lastStepTime;
        double lastStepDt;
        double lastStepDuration;
        double lastStepLoad;
        double totalTime;
		bool active;
        bool stopRequested;
		bool frameRequested;
		bool controllerRequested;
		bool lastFrameRequested;
        std::string endCommand;

        std::map<std::string, Controller*> controllers;
        Controller* activeController;
		std::string activeControllerName;

		ImageBuffer* classification;

		CRITICAL_SECTION socketMutex;
};

#endif // SOCCERBOT_H
