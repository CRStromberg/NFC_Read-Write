#include "writeTag.h"
#include "../display/display.h"
#include <ArduinoJson.h>

void writeToTag(Adafruit_PN532 &nfc) {
    Serial.println("Entered Write Mode");

    while (true) {  // Keep listening for JSON input
        Serial.println("Waiting for JSON input... (Send via Serial)");

        while (!Serial.available()) {
            delay(100);  // Keep waiting until data is received
        }

        String jsonInput = Serial.readStringUntil('\n');  // Read JSON from Serial
        StaticJsonDocument<1024> jsonDoc;
        DeserializationError error = deserializeJson(jsonDoc, jsonInput);

        if (error) {
            Serial.print("JSON Parsing Error: ");
            Serial.println(error.c_str());
            displayMessage("JSON Error");
            continue;  // Don't exit, keep listening for valid JSON
        }

        JsonObject nfcData = jsonDoc["nfc_data"];
        int totalTags = nfcData.size();
        int tagIndex = 0;

        for (JsonPair kv : nfcData) {
            tagIndex++;
            const char* tagName = kv.key().c_str();
            JsonArray tagData = kv.value().as<JsonArray>();

            Serial.print("\nWriting Tag: ");
            Serial.println(tagName);

            uint8_t uid[7], uidLength;
            if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
                Serial.println("No NFC tag detected.");
                displayMessage("No Tag");
                continue;
            }

            uint8_t keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

            if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 2, 0, keyA)) {
                Serial.println("Auth Failed for Block 2");
                displayMessage("Auth Failed");
                continue;
            }

            uint8_t tagDataArray[16] = {0};
            strncpy((char*)tagDataArray, tagName, min(strlen(tagName), 16));

            if (nfc.mifareclassic_WriteDataBlock(2, tagDataArray)) {
                Serial.println("Tag Name Written to Block 2");
            } else {
                Serial.println("Write Failed for Block 2");
                displayMessage("Write Failed");
                continue;
            }

            int blockIndex = 0;
            for (JsonVariant entry : tagData) {
                blockIndex++;
                JsonObject obj = entry.as<JsonObject>();
                for (JsonPair blockEntry : obj) {
                    int block = atoi(blockEntry.key().c_str());
                    String dataStr = blockEntry.value().as<String>();

                    Serial.print("Writing T: ");
                    Serial.print(tagIndex);
                    Serial.print("/");
                    Serial.print(totalTags);
                    Serial.print(" bl:");
                    Serial.println(block);

                    displayMessage(("T: " + String(tagIndex) + "/" + String(totalTags) + " bl:" + String(block)).c_str());

                    if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keyA)) {
                        Serial.println("Auth Failed for Block " + String(block));
                        continue;
                    }

                    uint8_t data[16] = {0};
                    int len = min(dataStr.length(), 16);
                    for (int i = 0; i < len; i++) {
                        data[i] = dataStr[i];
                    }

                    if (nfc.mifareclassic_WriteDataBlock(block, data)) {
                        Serial.println("Write OK");
                    } else {
                        Serial.println("Write Failed");
                    }
                }
            }
        }

        displayMessage("Done");

        // **Wait for new input, but don't exit loop**
        Serial.println("\nWaiting for the next JSON input...");
    }
}
