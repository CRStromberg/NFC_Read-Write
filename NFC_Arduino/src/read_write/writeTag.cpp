#include "writeTag.h"
#include "../display/display.h"
#include "../testing_debug/testing.h"
#include <ArduinoJson.h>


String readSerialData()
{
    String jsonInput = "";

    while (true)
    {
        while (Serial.available())
        {
            char c = Serial.read();
            jsonInput += c;
            delay(5); // Small delay to allow more data to arrive
        }

        // Check if we've received the full JSON (`}` followed by newline)
        if (jsonInput.endsWith("}\n"))
        {
            break; // Stop reading once the full JSON is received
        }

        delay(10); // Allow more data to arrive before checking again
    }

    return jsonInput;
}

void extractTagData(String jsonInput, Tag tags[], int &totalTags) {
    totalTags = 0;
    int start = jsonInput.indexOf("\"tags\": [") + 9;

    while (start != -1 && totalTags < MAX_TAGS) {
        start = jsonInput.indexOf("{", start);
        if (start == -1) break;

        int end = jsonInput.indexOf("}", start);
        String tagEntry = jsonInput.substring(start + 1, end);

        int keyStart = 0;
        int valueStart = 0;
        int pairIndex = 0;

        while ((keyStart = tagEntry.indexOf("\"", valueStart)) != -1 && pairIndex < 3) { // Max 3 per tag
            int keyEnd = tagEntry.indexOf("\"", keyStart + 1);
            int colon = tagEntry.indexOf(":", keyEnd);
            int valueStart = tagEntry.indexOf("\"", colon) + 1;
            int valueEnd = tagEntry.indexOf("\"", valueStart);

            if (keyEnd == -1 || colon == -1 || valueEnd == -1) break;

            tags[totalTags].block[pairIndex] = tagEntry.substring(keyStart + 1, keyEnd).toInt();
            tags[totalTags].data[pairIndex] = tagEntry.substring(valueStart, valueEnd);

            pairIndex++;
        }

        totalTags++;
        start = end;
    }
}

void writeToTag(Adafruit_PN532 &nfc)
{
    Serial.println(F("Entered Write Mode"));

    bool waiting_message = true;

    while (true)
    { // Keep listening for JSON input
        if (waiting_message)
        {
            Serial.println(F("Waiting for JSON input... (Send via Serial)"));
            waiting_message = false;
        }

        // Read full JSON before attempting to parse
        String jsonInput = readSerialData();
        if (jsonInput.length() == 0)
        {
            continue;
        }

        Serial.print(F("Received JSON Length: "));
        Serial.println(jsonInput.length());

        Serial.print(F("Raw JSON received: "));
        Serial.println(jsonInput);

        // Extract tag data into a struct
        Tag tags[MAX_TAGS];
        int totalTags = 0;

        extractTagData(jsonInput, tags, totalTags);

        printTagData(tags, totalTags);





        // int totalTags = nfcData.size();
        // int tagIndex = 0;

        // for (JsonPair kv : nfcData) {
        //     tagIndex++;
        //     const char* tagName = kv.key().c_str();
        //     JsonArray tagData = kv.value().as<JsonArray>();

        //     Serial.print(F("\nWriting Tag: "));
        //     Serial.println(tagName);

        //     uint8_t uid[7], uidLength;
        //     if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        //         Serial.println(F("No NFC tag detected."));
        //         displayMessage("No Tag");
        //         continue;
        //     }

        //     uint8_t keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        //     if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 2, 0, keyA)) {
        //         Serial.println(F("Auth Failed for Block 2"));
        //         displayMessage("Auth Failed");
        //         continue;
        //     }

        //     uint8_t tagDataArray[16] = {0};
        //     strncpy((char*)tagDataArray, tagName, min(strlen(tagName), 16));

        //     if (nfc.mifareclassic_WriteDataBlock(2, tagDataArray)) {
        //         Serial.println(F("Tag Name Written to Block 2"));
        //     } else {
        //         Serial.println(F("Write Failed for Block 2"));
        //         displayMessage("Write Failed");
        //         continue;
        //     }

        //     int blockIndex = 0;
        //     for (JsonVariant entry : tagData) {
        //         blockIndex++;
        //         JsonObject obj = entry.as<JsonObject>();
        //         for (JsonPair blockEntry : obj) {
        //             int block = atoi(blockEntry.key().c_str());
        //             String dataStr = blockEntry.value().as<String>();

        //             Serial.print(F("Writing T: "));
        //             Serial.print(tagIndex);
        //             Serial.print(F("/"));
        //             Serial.print(totalTags);
        //             Serial.print(F(" bl:"));
        //             Serial.println(block);

        //             displayMessage(("T: " + String(tagIndex) + "/" + String(totalTags) + " bl:" + String(block)).c_str());

        //             if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keyA)) {
        //                 Serial.println("Auth Failed for Block " + String(block));
        //                 continue;
        //             }

        //             uint8_t data[16] = {0};
        //             int len = min(dataStr.length(), 16);
        //             for (int i = 0; i < len; i++) {
        //                 data[i] = dataStr[i];
        //             }

        //             if (nfc.mifareclassic_WriteDataBlock(block, data)) {
        //                 Serial.println(F("Write OK"));
        //             } else {
        //                 Serial.println(F("Write Failed"));
        //             }
        //         }
        //     }
        // }

        displayMessage("Done");

        // **Wait for new input, but don't exit loop**
        Serial.println(F("\nWaiting for the next JSON input..."));
    }
}
