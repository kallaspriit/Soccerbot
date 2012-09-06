#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "websocketpp.hpp"

#include <pthread.h>
#include <set>

class WebSocketServer : public websocketpp::server::handler {
    public:
        class ListenerInterface {
            public:
                virtual void onSocketOpen(websocketpp::server::connection_ptr con) = 0;
                virtual void onSocketClose(websocketpp::server::connection_ptr con) = 0;
                virtual void onSocketMessage(websocketpp::server::connection_ptr con, websocketpp::server::handler::message_ptr msg) = 0;
        };

        WebSocketServer(int port);

        void start();
        void addListener(ListenerInterface* listener);
        void broadcast(std::string message);

    private:
        void listen();
        static void* _listen(void* context);
        void on_open(connection_ptr con);
        void on_close(connection_ptr con);
        void on_message(connection_ptr con, message_ptr msg);

        int port;
        websocketpp::server::handler::ptr handler;
        websocketpp::server server;
        pthread_t thread;
        std::set<ListenerInterface*> listeners;
        std::set<connection_ptr> connections;
};

#endif // WEBSOCKETSERVER_H
