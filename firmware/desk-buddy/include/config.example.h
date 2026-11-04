#pragma once

// =============================================================================
// Copy this file to "config.h" and customize with your network settings.
// =============================================================================

// Wi-Fi Configuration (for Web API & Agent Status updates)
#define WIFI_SSID           "YOUR_WIFI_SSID"
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"

// I2C OLED Pinout (ESP32 defaults)
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define OLED_ADDR           0x3C
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64

// Interactive Touch / Button Pin
// Touch pin (GPIO 4 is Touch0 on ESP32) or standard push button
#define TOUCH_PIN           4
#define TOUCH_THRESHOLD     40  // Capacitive touch threshold

// Companion Personality
#define PET_NAME            "Byte"
#define WEB_PORT            80
