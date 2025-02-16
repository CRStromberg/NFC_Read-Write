#include "testing.h"

void printTagData(Tag tags[], int totalTags) {
    Serial.print(F("\n Total Tags: "));
    Serial.println(totalTags);

    // Loop through tags dynamically
    for (int i = 0; i < totalTags; i++) {
        Serial.print(F("\nTag #"));
        Serial.println(i + 1);

        for (int j = 0; j < 3; j++) {
            // Skip empty blocks
            if (tags[i].block[j] == 0) continue;

            Serial.print(F("Block: "));
            Serial.print(tags[i].block[j]);
            Serial.print(F(" Data: "));
            Serial.println(tags[i].data[j]);
        }
    }
}
