#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>

#if __has_include("config.h")
    #include "config.h"
#else
    #include "config.example.h"
#endif

#include "bambu_client.h"
#include "ui_controller.h"

// Hardware instances
TFT_eSPI tft = TFT_eSPI();
UIController ui(tft);
BambuClient bambu;

// Dashboard State
DashboardPage currentPage = DashboardPage::BAMBU_PRINTER;
uint32_t lastPageSwitchMs = 0;
uint32_t lastUIRefreshMs = 0;

// Sample / Cached Market Data
StockItem stockData[] = {
    {"NVDA", 138.25, 4.12},
    {"AAPL", 224.50, -0.45},
    {"TSLA", 248.90, 2.30},
    {"BTC", 68520.0, 1.85}
};

// Sample / Cached Lab Health Nodes
LabNodeStatus labNodes[] = {
    {"Home Assistant", true, 4},
    {"Proxmox Hypervisor", true, 2},
    {"Synology NAS", true, 3},
    {"Bambu P1S Printer", true, 8}
};

void connectWiFi() {
    ui.showBootSplash("Connecting to Wi-Fi...");
    Serial.printf("[WiFi] Connecting to %s\n", WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Connected! IP: " + WiFi.localIP().toString());
        ui.showBootSplash("Wi-Fi Connected!");
    } else {
        Serial.println("\n[WiFi] Failed to connect (running in offline demo mode)");
        ui.showBootSplash("Offline Mode (Demo)");
    }
    delay(1000);
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== ESP32 Desk Dashboard Starting ===");

    // Initialize display and boot splash
    ui.begin();
    ui.showBootSplash("Starting System...");

    // Connect to network
    connectWiFi();

    // Initialize Bambu Lab Client if enabled
    #if BAMBU_ENABLED
    if (WiFi.status() == WL_CONNECTED) {
        bambu.begin(BAMBU_HOST, BAMBU_PORT, BAMBU_SERIAL, BAMBU_ACCESS_CODE);
    }
    #endif

    lastPageSwitchMs = millis();
}

void loop() {
    // Service background tasks
    #if BAMBU_ENABLED
    bambu.loop();
    #endif

    uint32_t now = millis();

    // Auto-rotate dashboard pages
    if (now - lastPageSwitchMs >= (SCREEN_ROTATE_SECS * 1000)) {
        lastPageSwitchMs = now;
        int nextPage = (static_cast<int>(currentPage) + 1) % static_cast<int>(DashboardPage::PAGE_COUNT);
        currentPage = static_cast<DashboardPage>(nextPage);
        lastUIRefreshMs = 0; // Force immediate re-render
    }

    // Refresh UI every 1000ms
    if (now - lastUIRefreshMs >= 1000) {
        lastUIRefreshMs = now;
        ui.render(
            currentPage,
            bambu.getState(),
            stockData, sizeof(stockData) / sizeof(stockData[0]),
            labNodes, sizeof(labNodes) / sizeof(labNodes[0])
        );
    }

    delay(20);
}
