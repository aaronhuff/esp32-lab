#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if __has_include("config.h")
    #include "config.h"
#else
    #include "config.example.h"
#endif

#include "pet_face.h"

// Hardware display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
PetFace pet(display);
WebServer server(WEB_PORT);

// Touch / Button state
bool lastTouchState = false;
uint32_t lastTouchCheckMs = 0;
int moodCycleIndex = 0;

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{font-family:sans-serif;text-align:center;background:#1e1e2e;color:#cdd6f4;padding:20px;}";
    html += "button{background:#89b4fa;color:#11111b;border:none;padding:12px 20px;margin:8px;font-size:16px;border-radius:8px;cursor:pointer;font-weight:bold;}";
    html += "button:hover{background:#b4befe;}</style></head><body>";
    html += "<h1>DeskBuddy Companion</h1>";
    html += "<p>Trigger Face & Reactions:</p>";
    html += "<button onclick=\"setMood('happy', 'Yay!')\">Happy</button>";
    html += "<button onclick=\"setMood('thinking', 'Agent Thinking...')\">Thinking</button>";
    html += "<button onclick=\"setMood('excited', 'Print Finished!')\">Excited</button>";
    html += "<button onclick=\"setMood('alert', 'Notice!')\">Alert</button>";
    html += "<button onclick=\"setMood('sleeping', 'Zzz...')\">Sleep</button>";
    html += "<button onclick=\"setMood('idle', '')\">Normal</button>";
    html += "<script>function setMood(m, txt){fetch('/api/mood',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({mood:m,msg:txt,duration:6000})});}</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleApiMood() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));
    if (err) {
        server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    String moodStr = doc["mood"] | "idle";
    const char* msg = doc["msg"] | "";
    uint32_t duration = doc["duration"] | 5000;

    PetMood newMood = PetMood::IDLE;
    if (moodStr == "happy") newMood = PetMood::HAPPY;
    else if (moodStr == "thinking") newMood = PetMood::THINKING;
    else if (moodStr == "sleeping") newMood = PetMood::SLEEPING;
    else if (moodStr == "excited") newMood = PetMood::EXCITED;
    else if (moodStr == "alert") newMood = PetMood::ALERT;
    else if (moodStr == "dizzy") newMood = PetMood::DIZZY;

    pet.setMood(newMood, strlen(msg) > 0 ? msg : nullptr, duration);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== DeskBuddy Starting ===");

    // Initialize I2C and Display
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("[Display] SSD1306 allocation failed");
    }

    pet.begin();
    pet.setMood(PetMood::HAPPY, "Hi there!", 2500);

    // WiFi Setup
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.printf("[WiFi] Connecting to %s", WIFI_SSID);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        delay(300);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Connected: " + WiFi.localIP().toString());
        server.on("/", HTTP_GET, handleRoot);
        server.on("/api/mood", HTTP_POST, handleApiMood);
        server.begin();
        Serial.println("[HTTP] Web interface active at http://" + WiFi.localIP().toString());
    } else {
        Serial.println("\n[WiFi] Offline (Touch interaction active)");
    }
}

void loop() {
    // Handle Web Server requests
    if (WiFi.status() == WL_CONNECTED) {
        server.handleClient();
    }

    // Touch / Button interaction check (every 50ms)
    uint32_t now = millis();
    if (now - lastTouchCheckMs >= 50) {
        lastTouchCheckMs = now;

        // On ESP32, touchRead(T0) returns low values when pin 4 is touched
        bool isTouched = false;
        #ifdef TOUCH_PIN
        int touchVal = touchRead(TOUCH_PIN);
        isTouched = (touchVal < TOUCH_THRESHOLD);
        #endif

        if (isTouched && !lastTouchState) {
            // Touch trigger event!
            moodCycleIndex = (moodCycleIndex + 1) % 4;
            if (moodCycleIndex == 0) pet.setMood(PetMood::HAPPY, "*giggles!*", 3000);
            else if (moodCycleIndex == 1) pet.setMood(PetMood::EXCITED, "Yay party!", 3000);
            else if (moodCycleIndex == 2) pet.setMood(PetMood::THINKING, "Hmm...", 3000);
            else pet.setMood(PetMood::ALERT, "Boop!", 2500);
            Serial.println("[Touch] Poked!");
        }
        lastTouchState = isTouched;
    }

    // Update eye animation frames (30-40 FPS)
    pet.update();
    delay(25);
}
