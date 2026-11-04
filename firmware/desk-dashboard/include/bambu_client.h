#pragma once
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "bambu_types.h"

class BambuClient {
public:
    BambuClient();
    void begin(const char* host, uint16_t port, const char* serial, const char* accessCode);
    void loop();
    const BambuState& getState() const { return state; }
    bool isConnected() { return mqttClient.connected(); }

private:
    void reconnect();
    void onMessage(char* topic, byte* payload, unsigned int length);
    void parseReport(JsonDocument& doc);

    const char* host = nullptr;
    uint16_t port = 8883;
    const char* serial = nullptr;
    const char* accessCode = nullptr;

    String topicReport;
    uint32_t lastReconnectAttempt = 0;

    WiFiClientSecure secureClient;
    PubSubClient mqttClient;
    BambuState state;
};
