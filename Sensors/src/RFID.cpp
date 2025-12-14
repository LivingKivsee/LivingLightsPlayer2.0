#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10  // SDA
#define RST_PIN 9  // Reset

#define OUTGPIO0 3
#define OUTGPIO1 4
#define OUTGPIO2 5

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    // Trigger output pins setup
    Serial.print("Trigger output pins set to: "); Serial.print(OUTGPIO2); Serial.print(" "); Serial.print(OUTGPIO1); Serial.print(" "); Serial.println(OUTGPIO0);
    Serial.println("Scan an NTAG215 sticker...");
    pinMode(OUTGPIO0, OUTPUT);
    pinMode(OUTGPIO1, OUTPUT);
    pinMode(OUTGPIO2, OUTPUT);
        digitalWrite(OUTGPIO0,  LOW);
        digitalWrite(OUTGPIO1,  LOW);
        digitalWrite(OUTGPIO2,  LOW);
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial()) return;

    Serial.print("Tag UID: ");
        
        digitalWrite(OUTGPIO0,  HIGH);
        digitalWrite(OUTGPIO1,  HIGH);
        digitalWrite(OUTGPIO2,  HIGH);
        delay(1000);
        digitalWrite(OUTGPIO0,  LOW);
        digitalWrite(OUTGPIO1,  LOW);
        digitalWrite(OUTGPIO2,  LOW);
        
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    char dataString[21] = ""; // 5 pages * 4 bytes = 20 chars + 1 for null terminator
    byte buffer[18];
    byte size = sizeof(buffer);
    byte index = 0;

    // Read NTAG215 memory pages 4-8
    for (byte page = 4; page <= 8; page++) {
        MFRC522::StatusCode status = rfid.MIFARE_Read(page, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print("Error reading page ");
            Serial.println(page);
            continue;
        }

        // Append the read bytes to dataString
        for (byte i = 0; i < 4; i++) {
            dataString[index++] = (char)buffer[i];
        }
    }
    dataString[index] = '\0'; // Ensure null termination

    Serial.print("Stored String: ");
    Serial.println(dataString); // Print as a string

    // Remove the last character
    int length = strlen(dataString);
    if (length > 0) {
        dataString[length - 1] = '\0';
    }

    Serial.print("Modified String: ");
    Serial.println(dataString);

    // Extract the last 4 digits and convert to integer
    if (length >= 5) { // Ensure there are at least 4 digits left
        char lastFourDigits[5];
        strncpy(lastFourDigits, dataString + length - 5, 4);
        lastFourDigits[4] = '\0';
        
        int extractedNumber = atoi(lastFourDigits);
        Serial.print("Extracted Integer: ");
        Serial.println(extractedNumber);
        //SendData(extractedNumber);
  
    } else {
        Serial.println("String too short to extract last 4 digits.");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

void SendData(int extractedNumber) {
  int output = extractedNumber % 8;  // תוצאה בין 0 ל-7
  digitalWrite(OUTGPIO0, (output & 1) ? HIGH : LOW);
  digitalWrite(OUTGPIO1, (output & 2) ? HIGH : LOW);
  digitalWrite(OUTGPIO2, (output & 4) ? HIGH : LOW);
  delay(1000);
}
