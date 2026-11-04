#!/usr/bin/env bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORT="${1:-/dev/ttyACM0}"

echo "Restoring factory flash image to ${PORT}..."
"${DIR}/../.venv/bin/esptool" --port "${PORT}" write_flash 0x0 "${DIR}/factory_backup_8mb.bin"
echo "Restore complete!"
