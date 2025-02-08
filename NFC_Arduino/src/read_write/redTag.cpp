#include "readTag.h"
#include "../display/display.h"

void readTag(Adafruit_PN532 &nfc) {
    uint8_t uid[7];
    uint8_t uidLength;

    Serial.println("Entered Read Mode.");

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.println("\nNFC Tag Detected!");
        uint8_t keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        bool isBlock2Empty = true;
        String block2Data = "";

        // Read Block 2 (Tag Identifier)
        if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 2, 0, keyA)) {
            uint8_t data[16];
            if (nfc.mifareclassic_ReadDataBlock(2, data)) {
                for (int i = 0; i < 16; i++) {
                    if (data[i] != 0) isBlock2Empty = false;
                    block2Data += (char)data[i];
                }
            }
        }

        // Handle Block 2 Display Logic
        if (isBlock2Empty) {
            displayMessage("Empty");
            delay(3000);
            displayMessage("Ready");
            return;
        } else {
            int loops = (block2Data.length() > 6) ? 2 : 1;
            for (int i = 0; i < loops; i++) {
                displayMessage(block2Data.c_str());
                delay(3000);
            }
        }

        // Read Other Blocks (Print to Serial Only)
        for (int block = 3; block <= 15; block++) {
            if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keyA)) {
                uint8_t data[16];
                if (nfc.mifareclassic_ReadDataBlock(block, data)) {
                    String readData = "";
                    bool isEmpty = true;

                    for (int i = 0; i < 16; i++) {
                        if (data[i] != 0) isEmpty = false;
                        readData += (char)data[i];
                    }

                    Serial.print("Block ");
                    Serial.print(block);
                    Serial.print(": ");
                    Serial.println(isEmpty ? "Empty" : readData);
                }
            }
        }
    }

    // After everything, reset the display to "Ready"
    displayMessage("Ready");
}
