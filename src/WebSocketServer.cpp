#include "WebSocketServer.h"

#include <cstring>

WebSocketServer::WebSocketServer(int port) : port(port), listening(false), handler(this), server(handler) {

}

WebSocketServer::~WebSocketServer() {
    close();
}

void WebSocketServer::listen() {
    try {
		server.start_listen(port);

		listening = true;
    } catch (const std::exception& e) {
        listening = false;

        std::cout << "- Failed to open socket server on port " << port << ": " << e.what() << std::endl;

        abort();
    }
}

void WebSocketServer::close() {
    if (!listening) {
        std::cout << "! Socket server is already closed" << std::endl;

        return;
    }

    //std::cout << "! Stopping the socket server" << std::endl;

    server.stop();

    //std::cout << "! Waiting for socket thread to join.. ";

    /*if (pthread_join(thread, NULL)) {
        std::cout << "- Could not join thread for socket" << std::endl;
    }*/

    //std::cout << "done!" << std::endl;

    listening = false;
}

void WebSocketServer::addListener(ListenerInterface* listener) {
    listeners.insert(listener);
}

void WebSocketServer::broadcast(std::string message) {
    if (!listening) {
        return;
    }

    //std::cout << "! Broadcast to " << connections.size() << ": " << message << std::endl;

    for (std::set<connection_ptr>::iterator it = connections.begin(); it != connections.end(); it++) {
        const connection_ptr con = *it;

        con->send(message);
    }
}

void WebSocketServer::on_open(connection_ptr con) {
    if (!listening) {
        return;
    }

    connections.insert(con);

    for (std::set<ListenerInterface*>::iterator iter = listeners.begin(); iter != listeners.end(); ++iter) {
        (*iter)->onSocketOpen(con);
    }
}

void WebSocketServer::on_close(connection_ptr con) {
    if (!listening) {
        return;
    }

    for (std::set<ListenerInterface*>::iterator iter = listeners.begin(); iter != listeners.end(); ++iter) {
        (*iter)->onSocketClose(con);
    }

    connections.erase(con);
}

void WebSocketServer::on_message(connection_ptr con, message_ptr msg) {
    if (!listening) {
        return;
    }

    for (std::set<ListenerInterface*>::iterator iter = listeners.begin(); iter != listeners.end(); ++iter) {
        (*iter)->onSocketMessage(con, msg);
    }
}
