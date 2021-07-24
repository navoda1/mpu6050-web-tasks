#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

class WebServer {

    public:
        WebServer(uint16_t port);
        void send_event(String event_id, String message);

    protected:
        AsyncWebServer *server;
        AsyncEventSource *events;
        
        void init_SPIFFS();
        void init_wifi();
        void init_server(uint16_t port);

    private:
        const char* ssid = "5W2GM";
        const char* password = "kandanasrilanka1";
};

#endif // __WEB_SERVER_H__