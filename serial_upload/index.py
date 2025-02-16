import serial
import time
import json

SERIAL_PORT = "/dev/tty.usbmodem1101"
BAUD_RATE = 115200
JSON_FILE = "../write_data/data.json"

def send_json():
    try:
        with open(JSON_FILE, "r") as file:
            json_data = file.read().strip()
    except FileNotFoundError:
        print(f"Error: File {JSON_FILE} not found.")
        return

    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
        time.sleep(2)

        print(f"Sending JSON data to {SERIAL_PORT} at {BAUD_RATE} baud...\n")
        print(json_data)

        # Send JSON **one character at a time** with a small delay
        for byte in json_data.encode('utf-8'):
            ser.write(bytes([byte]))
            # Delay to prevent overflow
            time.sleep(0.010)

        # Ensure newline is sent
        ser.write(b'\n')

        # Give Arduino time to process before reading response
        time.sleep(5)
        print("Arduino Response:")
        while ser.in_waiting:
            print(ser.readline().decode('utf-8').strip())

        ser.close()
        print("JSON data sent successfully.")

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {SERIAL_PORT}. Is your device connected?")
        print(e)

if __name__ == "__main__":
    send_json()
