#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

String incoming;

byte temp;
byte dectemp;
byte detect1;
byte detect2;
byte detect3;
byte gasdetect;
byte incomingLength;


void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  /* int packetSize = LoRa.parsePacket();
    if (packetSize) {
     while (LoRa.available()) {
       incoming = (char)LoRa.read();
       Serial.print(incoming);
       lcd.print(incoming);
     }
     Serial.print("' with RSSI ");
     Serial.println(LoRa.packetRssi());
    }*/
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;
  temp = LoRa.read();
  dectemp = LoRa.read();
  detect1 = LoRa.read();
  detect2 = LoRa.read();
  detect3 = LoRa.read();
  gasdetect = LoRa.read();  
  /*if (incomingLength != incoming.length()) {
    Serial.println("error: message length does not match length");
    return;
    }*/
  Serial.println("Temperature: " + String(temp) + "." + String(dectemp));
  Serial.println("Motion Detect 1: " + String(detect1));
  Serial.println("Motion Detect 2: " + String(detect2));
  Serial.println("Motion Detect 3: " + String(detect3));
     
  if (String (detect1) == "0" && String (detect2) == "0" && String (detect3) == "0") {
    lcd.setCursor(0, 1);
    lcd.print("No Motion Detect");
  } else if (String (detect1) == "1" || String (detect2) == "0" || String (detect3) == "0") {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Motion Detect");
  }
  else if (String (detect1) == "0" || String (detect2) == "1" || String (detect3) == "0") {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Motion Detect");
  }
  else if (String (detect1) == "0" || String (detect2) == "0" || String (detect3) == "1"){
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Motion Detect");
  }
  lcd.setCursor(0,0);
  lcd.print("Temp" + String(temp) + "." + String(dectemp));
  lcd.setCursor(10,0);
  lcd.print("Gas"+ String(gasdetect));
  }
