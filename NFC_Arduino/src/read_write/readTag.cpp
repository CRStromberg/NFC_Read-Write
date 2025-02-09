#include "readTag.h"
#include "../display/display.h"

// Define amount of Blocks to read
const int BLOCK_SIZE = 4;  // User can modify this value

void readTag(Adafruit_PN532 &nfc) {
    uint8_t uid[7];
    uint8_t uidLength;

    Serial.println(F("Entered Read Mode."));

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.println(F("\nNFC Tag Detected!"));
        uint8_t keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        bool isBlock1Empty = true;
        String block1Data = "";

        // 🔍 Read Block 1 (Tag Identifier)
        if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 1, 0, keyA)) {
            uint8_t data[16];
            if (nfc.mifareclassic_ReadDataBlock(1, data)) {
                for (int i = 0; i < 16; i++) {
                    if (data[i] != 0) isBlock1Empty = false;
                    block1Data += (char)data[i];
                }
            }
        } else {
            Serial.println(F("Auth Failed for Block 1"));
        }

        // 🔍 Handle Block 1 Display Logic
        if (isBlock1Empty) {
            displayMessage("Empty");
            delay(3000);
        } else {
            int loops = (block1Data.length() > 6) ? 2 : 1;
            for (int i = 0; i < loops; i++) {
                displayMessage(block1Data.c_str());
                delay(3000);
            }
        }

        // 🔍 Read Other Blocks (Print to Serial Only)
        int lastAuthenticatedSector = -1;  // Track last authenticated sector

        for (int block = 2; block <= BLOCK_SIZE; block++) {
            int sector = block / 4;  // Each sector has 4 blocks
            int trailerBlock = (sector * 4) + 3;  // Calculate sector trailer block

            // ❌ Skip sector trailer blocks (3, 7, 11, 15...)
            if (block == trailerBlock) {
                Serial.print(F("Skipping Sector Trailer Block: "));
                Serial.println(block);
                continue;
            }

            // 🔑 Authenticate if entering a new sector
            if (sector != lastAuthenticatedSector) {
                Serial.print(F("Authenticating for Sector "));
                Serial.println(sector);

                if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keyA)) {
                    Serial.print(F("Auth Failed for Block "));
                    Serial.println(block);
                    lastAuthenticatedSector = -1;  // Reset authentication status
                    continue;  // Skip to the next block
                }
                lastAuthenticatedSector = sector;  // Store last authenticated sector
            }

            uint8_t data[16];
            if (!nfc.mifareclassic_ReadDataBlock(block, data)) {
                Serial.print(F("Failed to read Block "));
                Serial.println(block);
                continue;  // Skip to the next block
            }

            String readData = "";
            bool isEmpty = true;

            for (int i = 0; i < 16; i++) {
                if (data[i] != 0) isEmpty = false;
                readData += (char)data[i];
            }

            Serial.print(F("Block "));
            Serial.print(block);
            Serial.print(F(": "));
            Serial.println(isEmpty ? F("Empty") : readData);
        }
    } else {
        Serial.println(F("No NFC tag detected."));
    }

    // 🔄 Reset the display to "Ready" AFTER reading all blocks
    displayMessage("Ready");
}
