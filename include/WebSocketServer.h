#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "websocketpp.hpp"

#include "Thread.h"

#include <set>

class WebSocketServer : public websocketpp::server::handler, private Thread {
    public:
        class ListenerInterface {
            public:
                virtual void onSocketOpen(websocketpp::server::connection_ptr con) = 0;
                virtual void onSocketClose(websocketpp::server::connection_ptr con) = 0;
                virtual void onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg) = 0;
        };

        WebSocketServer(int port);
        virtual ~WebSocketServer();

        void listen();
        void close();
        void addListener(ListenerInterface* listener);
        void broadcast(std::string message);

    private:
        virtual void* run();

        void on_open(connection_ptr con);
        void on_close(connection_ptr con);
        void on_message(connection_ptr con, message_ptr msg);

        int port;
        bool listening;
        websocketpp::server::handler::ptr handler;
        websocketpp::server server;
        std::set<ListenerInterface*> listeners;
        std::set<connection_ptr> connections;
};

#endif // WEBSOCKETSERVER_H
