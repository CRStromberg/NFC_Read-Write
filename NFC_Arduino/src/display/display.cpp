#include "display.h"

void displayMessage(const char *message) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);

    int messageLength = strlen(message);
    int visibleChars = 6;

    if (messageLength <= visibleChars) {
        display.println(message);
    } else {
        for (int i = 0; i < messageLength + visibleChars; i++) {
            display.clearDisplay();
            display.setCursor(10, 10);
            for (int j = 0; j < visibleChars; j++) {
                int charIndex = (i + j) % messageLength;
                display.print(message[charIndex]);
            }
            display.display();
            delay(500);
        }
    }

    display.display();
}
