#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <MD_MAX72xx.h>

#define SS_PIN   5
#define RST_PIN  22
MFRC522 rfid(SS_PIN, RST_PIN);

#define MAX_DEVICES 1
#define CS_PIN 15
MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::FC16_HW, CS_PIN, MAX_DEVICES);

byte tick[8] = {
  B00000000,
  B00000001,
  B00000010,
  B00000100,
  B10001000,
  B01010000,
  B00100000,
  B00000000
};

byte cross[8] = {
  B10000001,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B10000001
};

void drawPattern(byte pattern[8]) {
  mx.clear();
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      bool state = pattern[row] & (1 << col);
      mx.setPoint(7 - row, col, state);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 5);
  mx.clear();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String kartUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) kartUID += "0";
    kartUID += String(rfid.uid.uidByte[i], HEX);
  }
  kartUID.toUpperCase();

  if (kartUID == "467B71B8") {
    drawPattern(tick);
  } else {
    drawPattern(cross);
  }

  delay(2000);
  mx.clear();
  rfid.PICC_HaltA();
}
