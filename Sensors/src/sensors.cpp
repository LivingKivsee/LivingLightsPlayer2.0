#include <Arduino.h>

#define OUTGPIO0 3
#define OUTGPIO1 4
#define OUTGPIO2 5

void setup() {
    Serial.begin(115200);
    Serial.println("Serial Port Started.");

    // Trigger output pins setup
    pinMode(OUTGPIO0, OUTPUT);
    pinMode(OUTGPIO1, OUTPUT);
    pinMode(OUTGPIO2, OUTPUT);
    Serial.print("Trigger output pins set to: "); Serial.print(OUTGPIO2); Serial.print(" "); Serial.print(OUTGPIO1); Serial.print(" "); Serial.println(OUTGPIO0);

}

void loop() {
    // loop through all 8 possible combinations of the 3 output pins
    for (int i = 0; i < 8; i++) {
        digitalWrite(OUTGPIO0, i & 1);
        digitalWrite(OUTGPIO1, i & 2);
        digitalWrite(OUTGPIO2, i & 4);
        delay(10000);
    }
}
