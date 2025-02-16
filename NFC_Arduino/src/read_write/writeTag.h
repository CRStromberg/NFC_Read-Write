#ifndef WRITE_TAG_H
#define WRITE_TAG_H

#include <Adafruit_PN532.h>


#define MAX_TAGS 10  // Set max number of tags (adjust as needed)
struct Tag {
    int block[3];  // Holds block numbers like 1, 2, 5, etc.
    String data[3]; // Holds corresponding values like "red", "255-0-0", etc.
};

void extractTagData(String jsonInput, Tag tags[], int &totalTags);

void writeToTag(Adafruit_PN532 &nfc);

#endif
