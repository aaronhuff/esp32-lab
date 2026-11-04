#include "bambu_client.h"

BambuClient::BambuClient() : mqttClient(secureClient) {}

void BambuClient::begin(const char* host, uint16_t port, const char* serial, const char* accessCode) {
    this->host = host;
    this->port = port;
    this->serial = serial;
    this->accessCode = accessCode;

    this->topicReport = String("device/") + serial + "/report";

    // Bambu printers use a self-signed TLS certificate
    secureClient.setInsecure();

    // Bambu status reports are detailed JSON payloads (typically 2-3KB)
    mqttClient.setBufferSize(4096);
    mqttClient.setServer(this->host, this->port);

    mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->onMessage(topic, payload, length);
    });

    Serial.printf("[Bambu] Configured for host: %s, Serial: %s\n", host, serial);
}

void BambuClient::loop() {
    if (!mqttClient.connected()) {
        uint32_t now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            reconnect();
        }
    } else {
        mqttClient.loop();
    }
}

void BambuClient::reconnect() {
    if (!host || !serial || !accessCode) return;
    if (WiFi.status() != WL_CONNECTED) return;

    Serial.print("[Bambu] Attempting MQTT connection... ");
    String clientId = "ESP32-Dashboard-" + String(random(0xffff), HEX);

    // Bambu local MQTT credentials: username is always "bblp", password is the 8-digit access code
    if (mqttClient.connect(clientId.c_str(), "bblp", accessCode)) {
        Serial.println("CONNECTED!");
        state.connected = true;
        mqttClient.subscribe(topicReport.c_str());
        Serial.printf("[Bambu] Subscribed to %s\n", topicReport.c_str());
    } else {
        state.connected = false;
        Serial.printf("FAILED (rc=%d)\n", mqttClient.state());
    }
}

void BambuClient::onMessage(char* topic, byte* payload, unsigned int length) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
        Serial.printf("[Bambu] JSON parse failed: %s\n", error.c_str());
        return;
    }

    parseReport(doc);
}

void BambuClient::parseReport(JsonDocument& doc) {
    JsonObject print = doc["print"];
    if (print.isNull()) return;

    // Execution state (IDLE, RUNNING, PAUSE, FINISH, FAILED)
    if (print["gcode_state"].is<const char*>()) {
        state.gcode_state = print["gcode_state"].as<String>();
    }

    // Print percentage (0 - 100)
    if (print["mc_percent"].is<int>()) {
        state.mc_percent = print["mc_percent"].as<int>();
    }

    // Remaining time in minutes
    if (print["mc_remaining_time"].is<int>()) {
        state.mc_remaining_time = print["mc_remaining_time"].as<int>();
    }

    // Temperatures
    if (print["nozzle_temper"].is<float>()) {
        state.nozzle_temper = print["nozzle_temper"].as<float>();
    }
    if (print["nozzle_target_temper"].is<float>()) {
        state.nozzle_target_temper = print["nozzle_target_temper"].as<float>();
    }
    if (print["bed_temper"].is<float>()) {
        state.bed_temper = print["bed_temper"].as<float>();
    }
    if (print["bed_target_temper"].is<float>()) {
        state.bed_target_temper = print["bed_target_temper"].as<float>();
    }
    if (print["chamber_temper"].is<float>()) {
        state.chamber_temper = print["chamber_temper"].as<float>();
    }

    // Layer info
    if (print["layer_num"].is<int>()) {
        state.current_layer = print["layer_num"].as<int>();
    }
    if (print["total_layer_num"].is<int>()) {
        state.total_layers = print["total_layer_num"].as<int>();
    }

    // Active subtask / filename
    if (print["subtask_name"].is<const char*>()) {
        const char* name = print["subtask_name"].as<const char*>();
        if (name && strlen(name) > 0) {
            state.subtask_name = String(name);
        }
    }

    // Fan cooling speed
    if (print["cooling_fan_speed"].is<int>()) {
        state.fan_gear = print["cooling_fan_speed"].as<int>();
    }

    state.last_update_ms = millis();
}
