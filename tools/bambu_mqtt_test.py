#!/usr/bin/env python3
"""
Bambu Lab Local MQTT Test Utility
Verifies connectivity and dumps live printer telemetry.
Run:
    python tools/bambu_mqtt_test.py --host <PRINTER_IP> --serial <SERIAL> --code <ACCESS_CODE>
"""

import argparse
import json
import ssl
import sys
import time

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("paho-mqtt is required. Run: pip install paho-mqtt")
    sys.exit(1)


def parse_args():
    parser = argparse.ArgumentParser(description="Test Bambu Lab Local MQTT Connection")
    parser.add_argument("--host", required=True, help="Printer IP address (e.g. 192.168.1.150)")
    parser.add_argument("--port", type=int, default=8883, help="MQTT SSL port (default: 8883)")
    parser.add_argument("--serial", required=True, help="Printer serial number (e.g. 01P00A123456789)")
    parser.add_argument("--code", required=True, help="8-digit LAN access code from printer settings")
    return parser.parse_args()


def main():
    args = parse_args()
    topic = f"device/{args.serial}/report"

    print(f"[*] Connecting to Bambu Lab printer at {args.host}:{args.port}...")
    print(f"[*] Target Serial: {args.serial}")
    print(f"[*] Subscribing to: {topic}\n")

    client = mqtt.Client(client_id="BambuTester")
    client.username_pw_set("bblp", args.code)

    # Bambu printers use a self-signed TLS cert on port 8883
    client.tls_set(cert_reqs=ssl.CERT_NONE)
    client.tls_insecure_set(True)

    def on_connect(c, userdata, flags, rc):
        if rc == 0:
            print("[+] Successfully connected to printer MQTT broker!")
            client.subscribe(topic)
        else:
            print(f"[-] Connection failed with result code: {rc}")

    def on_message(c, userdata, msg):
        try:
            payload = json.loads(msg.payload.decode("utf-8"))
            print_data = payload.get("print", {})
            if not print_data:
                return

            state = print_data.get("gcode_state", "UNKNOWN")
            pct = print_data.get("mc_percent", 0)
            remaining_mins = print_data.get("mc_remaining_time", 0)
            nozzle = print_data.get("nozzle_temper", 0)
            bed = print_data.get("bed_temper", 0)
            subtask = print_data.get("subtask_name", "N/A")

            print(
                f"[TELEMETRY] State: {state:8} | Progress: {pct:3}% ({remaining_mins}m left) | "
                f"Nozzle: {nozzle:5.1f}C | Bed: {bed:4.1f}C | Model: {subtask}"
            )
        except Exception as e:
            print(f"[!] Parse error: {e}")

    client.on_connect = on_connect
    client.on_message = on_message

    try:
        client.connect(args.host, args.port, keepalive=60)
        client.loop_forever()
    except KeyboardInterrupt:
        print("\nExiting...")
        client.disconnect()


if __name__ == "__main__":
    main()
