import os
import serial
import time
import json

# Load configuration from `config.json`
CONFIG_FILE = "config.json"

try:
    with open(CONFIG_FILE, "r") as file:
        config = json.load(file)
except FileNotFoundError:
    print(f"Error: Config file {CONFIG_FILE} not found.")
    exit(1)

# Extract values from config
SERIAL_PORT = config.get("arduino_address", "/dev/ttyUSB0")
BAUD_RATE = config.get("baud_rate", 115200)
JSON_FILE = "write_data/nfc_data.json"  # Ensure this is in `write_data/`

def send_json():
    try:
        with open(JSON_FILE, "r") as file:
            json_data = file.read()
    except FileNotFoundError:
        print(f"Error: File {JSON_FILE} not found.")
        return

    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
        time.sleep(2)  # Wait for Arduino to reset

        print(f"Sending JSON data to {SERIAL_PORT} at {BAUD_RATE} baud...\n")
        print(json_data)

        ser.write(json_data.encode('utf-8'))
        ser.write(b'\n')

        ser.close()
        print("✅ JSON data sent successfully.")

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {SERIAL_PORT}. Is your device connected?")
        print(e)

if __name__ == "__main__":
    send_json()
