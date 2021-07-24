#include "WebServer.h"

WebServer::WebServer(uint16_t port) {
    /* Constructor */

    // Initialize file system
    init_SPIFFS();

    // Connect to Wifi
    init_wifi();

    // Initialize and start the server
    init_server(port);

}

void WebServer::init_wifi() {
    /* Initialize the library and connect to the Wifi network */

    WiFi.mode(WIFI_STA);
    WiFi.begin(this->ssid, this->password);
    
    Serial.print("\nConnecting to WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("\nWifi Connected. IP address: ");
    Serial.print(WiFi.localIP());
}

void WebServer::init_SPIFFS() {
    /* Check if SPIFFS is mounted properly */

    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    else {
        Serial.println("SPIFFS mounted successfully");
    }
}

void WebServer::init_server(uint16_t port) {
    // initialize the async server
    server = new AsyncWebServer(port);

    // Create an Event Source on /events
    events = new AsyncEventSource("/events");

    // Serve the index.html file on GET requests to the base URL
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server->serveStatic("/", SPIFFS, "/");

    // Handle Web Server Events
    events->onConnect([](AsyncEventSourceClient *client){
        if(client->lastId()){
            Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
        }
        // send event with message "hello!", id current millis
        // and set reconnect delay to 1 second
        client->send("hello!", NULL, millis(), 10000);
    });

    server->addHandler(events);

    server->begin();
}

void WebServer::send_event(String event_id, String message) {
    events->send(message.c_str(), event_id.c_str(), millis());
}