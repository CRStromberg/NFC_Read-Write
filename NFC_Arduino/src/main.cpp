#include <Wire.h>
#include <Adafruit_PN532.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "read_write/readTag.h"
#include "read_write/writeTag.h"
#include "display/display.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define PN532_IRQ     2
#define PN532_RESET   3
#define WRITE_TRIGGER_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

bool isWritingMode = false;  // Tracks the current mode

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(100000);
    pinMode(WRITE_TRIGGER_PIN, INPUT_PULLUP);

    Serial.println(F("Initializing PN532..."));
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println(F("PN532 not detected! Check wiring."));
        return;
    }

    Serial.print(F("PN532 Firmware: 0x"));
    Serial.println(versiondata, HEX);
    nfc.SAMConfig();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        return;
    }

    displayMessage("Ready");
}

void loop() {
    bool buttonState = digitalRead(WRITE_TRIGGER_PIN) == LOW;

    // Switch modes when button state changes
    if (buttonState && !isWritingMode) {
        Serial.println(F("Switched to Write Mode"));
        isWritingMode = true;
        delay(1000);  // Small delay to prevent rapid switching
    } else if (!buttonState && isWritingMode) {
        Serial.println(F("Switched to Read Mode"));
        isWritingMode = false;
        delay(1000);  // Small delay to prevent rapid switching
    }

    // Run the appropriate function based on mode
    if (isWritingMode) {
        writeToTag(nfc);
    } else {
        readTag(nfc);
    }
}
