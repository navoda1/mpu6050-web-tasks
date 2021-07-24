#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>

#include "WebServer.h"
#include "MPU6050.h"

#define ACCEL_QUEUE_SIZE 5

// global variables
WebServer *web_server;
MPU6050 *mpu_6050;
static QueueHandle_t accel_dqueue;
char json_buffer[30];

void accel_read_task(void *parameter) {
    mpu_6050 = new MPU6050(0x68);
    int16_t accel_reading[3];

    while (1) {
        // get the accelerator readings and print them to the serial monitor
        mpu_6050->get_accel_reading(accel_reading);
        Serial.print("Accelerometer: ");
        Serial.print(accel_reading[0]); Serial.print(" | ");
        Serial.print(accel_reading[1]); Serial.print(" | ");
        Serial.println(accel_reading[2]);

        // Push the latest data into the queue (non-blocking)
        xQueueSend(accel_dqueue, (void *) accel_reading, (TickType_t) 0);

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void server_update(void *parameter) {

    int16_t accel_reading[3];

    while (1) {

        if (xQueueReceive(accel_dqueue, accel_reading, (TickType_t) 400 / portTICK_PERIOD_MS) == pdPASS ) {
            // convert readings to json string
            StaticJsonDocument<48> doc;
            copyArray(accel_reading, doc.to<JsonArray>());
            serializeJson(doc, json_buffer);

            // send the event to the browser
            web_server->send_event("accel_readings", json_buffer);
        } 
        else {
            Serial.println("WARNING:server_update:No accelerator data");
        }

    }

}

void setup() {
    // Configure Serial
    Serial.begin (115200);

    // Create queues
    accel_dqueue = xQueueCreate(ACCEL_QUEUE_SIZE, sizeof(int16_t) * 3); /* Each reading has 3 int16_t values */
    
    // Create and start the web server
    web_server = new WebServer(80);

    // Create the accelerometer data reading task
    xTaskCreate(
        accel_read_task,        /* Task function. */
        "accel_read_task",      /* String with name of task. */
        5000,                   /* Stack size in bytes. */
        NULL,                   /* Parameter passed as input of the task */
        1,                      /* Priority of the task. */
        NULL                    /* Task handle. */
    );

    // Create server update task
    xTaskCreate(
        server_update,          /* Task function. */
        "server_update",        /* String with name of task. */
        5000,                   /* Stack size in bytes. */
        NULL,                   /* Parameter passed as input of the task */
        1,                      /* Priority of the task. */
        NULL                    /* Task handle. */
    );
}

void loop()
{

}
