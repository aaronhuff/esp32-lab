#!/usr/bin/env python3
"""
DeskBuddy Remote Controller CLI
Trigger facial expressions, status moods, and speech messages on your DeskBuddy.
Run:
    python tools/pet_controller.py --host 192.168.1.180 --mood happy --msg "Code built!"
"""

import argparse
import json
import sys
import urllib.request


def parse_args():
    parser = argparse.ArgumentParser(description="DeskBuddy Companion Controller")
    parser.add_argument("--host", required=True, help="ESP32 IP address or hostname")
    parser.add_argument(
        "--mood",
        choices=["happy", "thinking", "sleeping", "excited", "alert", "dizzy", "idle"],
        default="happy",
        help="Facial mood",
    )
    parser.add_argument("--msg", default="", help="Optional text message for speech bubble")
    parser.add_argument("--duration", type=int, default=5000, help="Duration in milliseconds")
    return parser.parse_args()


def main():
    args = parse_args()
    url = f"http://{args.host}/api/mood"
    payload = {
        "mood": args.mood,
        "msg": args.msg,
        "duration": args.duration,
    }

    req = urllib.request.Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
        method="POST",
    )

    print(f"[*] Sending mood '{args.mood}' to DeskBuddy at {url}...")
    try:
        with urllib.request.urlopen(req, timeout=5) as resp:
            if resp.status == 200:
                print(f"[+] Success! DeskBuddy updated.")
            else:
                print(f"[-] Received HTTP status {resp.status}")
    except Exception as e:
        print(f"[!] Failed to connect: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
