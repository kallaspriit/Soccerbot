#include "WebSocketServer.h"

#include <cstring>

WebSocketServer::WebSocketServer(int port) : port(port), handler(this), server(handler) {

}

void WebSocketServer::listen() {
    std::cout << "! Socket server is now listening" << std::endl;

    server.listen(port);
}

void* WebSocketServer::_listen(void* context)  {
    ((WebSocketServer*)context)->listen();

    return 0;
}

void WebSocketServer::start() {
    pthread_create(&thread, NULL, &WebSocketServer::_listen, this);
}

void WebSocketServer::addListener(ListenerInterface* listener) {
    listeners.insert(listener);
}

void WebSocketServer::broadcast(std::string message) {
    //std::cout << "! Broadcast to " << connections.size() << ": " << message << std::endl;

    for (std::set<connection_ptr>::iterator it = connections.begin(); it != connections.end(); it++) {
        const connection_ptr con = *it;

        con->send(message);
    }
}

void WebSocketServer::on_open(connection_ptr con) {
    connections.insert(con);

    for (std::set<ListenerInterface*>::iterator iter = listeners.begin(); iter != listeners.end(); ++iter) {
        (*iter)->onSocketOpen(con);
    }
}

void WebSocketServer::on_close(connection_ptr con) {
    for (std::set<ListenerInterface*>::iterator iter = listeners.begin(); iter != listeners.end(); ++iter) {
        (*iter)->onSocketClose(con);
    }

    connections.erase(con);
}

void WebSocketServer::on_message(connection_ptr con, message_ptr msg) {
    for (std::set<ListenerInterface*>::iterator iter = listeners.begin(); iter != listeners.end(); ++iter) {
        (*iter)->onSocketMessage(con, msg);
    }
}
