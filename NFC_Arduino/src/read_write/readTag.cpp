#include "readTag.h"
#include "../display/display.h"

// Define amount of Blocks to read
const int SECTOR = 0;

void readTag(Adafruit_PN532 &nfc)
{
    uint8_t uid[7];
    uint8_t uidLength;
    int starting_block = (SECTOR == 0) ? 1 : (SECTOR * 4);
    int loop_count  = (SECTOR == 0) ? 2 : 3;
    // Set loop_count To Serial Output Blocks


    Serial.println(F("Entered Read Mode."));

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength))
    {
        Serial.println(F("\nNFC Tag Detected!"));
        uint8_t keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        bool Empty[3] = {true, true, true};
        String blockData[3];

        // Read Block 1 (Tag Identifier)
        if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, starting_block, 0, keyA))
        {
            uint8_t data[16];
            if (nfc.mifareclassic_ReadDataBlock(starting_block, data))
            {
                for (int i = 0; i < 16; i++)
                {
                    if (data[i] != 0)
                        Empty[0] = false;
                    blockData[0] += (char)data[i];
                }
            }

            // Read remaining blocks
            for (int loop = 0; loop < loop_count; loop++)
            {
                int block_to_read = starting_block + loop;
                if (nfc.mifareclassic_ReadDataBlock(block_to_read, data))
                {
                    for (int i = 0; i < 16; i++)
                    {
                        if (data[i] != 0)
                            Empty[loop+1] = false;
                        blockData[loop+1] += (char)data[i];
                    }
                }
            }
        }
        else
        {
            Serial.println(F("Auth Failed For Sector "));
            Serial.println(SECTOR);
        }

        // Check if first writeable block is empty
        if (Empty[0])
        {
            displayMessage("Empty");
            delay(3000);
        }
        else
        {
            int loops = (blockData[0].length() > 6) ? 2 : 1;
            for (int i = 0; i < loops; i++)
            {
                displayMessage(blockData[0].c_str());
                delay(3000);
            }
        }

        // Log data from remaining blocks
        for (int loop = 1; loop < loop_count; loop++)
        {
            Serial.println(F("Block "));
            Serial.println(starting_block+loop);
            Serial.println(F(": "));
            if (Empty[loop])
            {
                Serial.println(F("Empty"));
            }
            else
            {
                Serial.println(blockData[loop].c_str());
            }
        }
    }
    else
    {
        Serial.println(F("No NFC tag detected."));
    }

    // Avoid rapid rescan
    delay(2000);

    // Reset the display to "Ready" AFTER reading all blocks
    displayMessage("Ready");
}
