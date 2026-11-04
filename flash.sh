#!/usr/bin/env bash
set -e

cd "$(dirname "$0")"

if [ ! -d ".venv" ]; then
    echo "Virtual environment not found! Setting up..."
    /usr/bin/python3 -m venv .venv
    .venv/bin/pip install esphome
fi

echo "========================================="
echo " Flashing ESP32-C6 BTC Ticker (ESPHome) "
echo "========================================="

# Check secrets.yaml
if grep -q "YOUR_WIFI_SSID" secrets.yaml 2>/dev/null; then
    echo "[!] WARNING: Please update your Wi-Fi credentials in esp32-lab/secrets.yaml before flashing."
fi

# Run compile & upload
.venv/bin/esphome run btc-ticker.yaml --device /dev/ttyACM0
