#pragma once

// =============================================================================
// Copy this file to "config.h" and fill in your actual credentials.
// "config.h" is ignored by git to protect your secrets.
// =============================================================================

// Wi-Fi Configuration
#define WIFI_SSID           "YOUR_WIFI_SSID"
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"

// Bambu Lab 3D Printer Configuration
// Notes:
// 1. IP address: Find on the printer's screen under Settings -> Wi-Fi
// 2. Serial Number: Under Settings -> Device Info (e.g. 01P00A123456789)
// 3. Access Code: 8-digit passcode found under Settings -> Wi-Fi / Local Network
#define BAMBU_ENABLED       true
#define BAMBU_HOST          "192.168.1.150"
#define BAMBU_PORT          8883
#define BAMBU_SERIAL        "01P00A123456789"
#define BAMBU_ACCESS_CODE   "12345678"

// Stock / Crypto Tickers (Comma-separated or configured via API)
#define STOCKS_ENABLED      true
#define STOCK_SYMBOLS       {"NVDA", "AAPL", "TSLA", "BTC"}
#define FINNHUB_API_KEY     "YOUR_FINNHUB_KEY" // Optional: Free tier at finnhub.io

// Lab Health / Home Assistant Integration (Optional)
#define LAB_HEALTH_ENABLED  false
#define HA_BASE_URL         "http://homeassistant.local:8123"
#define HA_TOKEN            "YOUR_LONG_LIVED_ACCESS_TOKEN"

// UI Behavior
#define SCREEN_ROTATE_SECS  8      // Auto-switch between dashboard pages every N seconds
#define TIMEZONE_OFFSET_HRS -5     // EST = -5, CST = -6, PST = -8
#define NTP_SERVER          "pool.ntp.org"
