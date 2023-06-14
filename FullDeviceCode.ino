#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <BluetoothSerial.h>

//GPS
static const int RXPin = 25, TXPin = 32;
static const uint32_t GPSBaud = 9600;

// Motor A
int motor1Pin1 = 13;
int motor1Pin2 = 12;
int enable1Pin = 14;

// Setting PWM properties
const int freq = 100;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 120;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

BluetoothSerial SerialBT;

void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);

  Serial.begin(9600);
  ss.begin(GPSBaud);

  SerialBT.begin("ESP32 Bluetooth full device");  // Bluetooth name
}

void loop() {

  if (SerialBT.available()) {
    char command = SerialBT.read();


    //Blocking device
    if (command == '1') {
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      ledcWrite(pwmChannel, 170);
      SerialBT.println("Device is blocked");
      for (int i = 170; i > 100; i = i - 5) {
        ledcWrite(pwmChannel, i);
        delay(100);
      }
      delay(1000);
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);


      //Unblocking device
    } else if (command == '0') {
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);
      ledcWrite(pwmChannel, 170);
      SerialBT.println("Device is unblocked");
      for (int i = 170; i > 100; i = i - 5) {
        ledcWrite(pwmChannel, i);
        delay(100);
      }
      delay(1000);
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);
    }
    sendLocationOverBluetooth();
    delay(1500);
  }

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      ;
  }
}

// Test part,can be used instead sendLocationOverBluetooth
void displayInfoBluetooth() {
  SerialBT.print(F("Location: "));
  if (gps.location.isValid()) {
    SerialBT.print(gps.location.lat(), 6);
    SerialBT.print(F(","));
    SerialBT.print(gps.location.lng(), 6);
  } else {
    SerialBT.print(F("INVALID"));
  }

  SerialBT.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    SerialBT.print(gps.date.month());
    SerialBT.print(F("/"));
    SerialBT.print(gps.date.day());
    SerialBT.print(F("/"));
    SerialBT.print(gps.date.year());
  } else {
    SerialBT.print(F("INVALID"));
  }

  SerialBT.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) SerialBT.print(F("0"));
    SerialBT.print(gps.time.hour());
    SerialBT.print(F(":"));
    if (gps.time.minute() < 10) SerialBT.print(F("0"));
    SerialBT.print(gps.time.minute());
    SerialBT.print(F(":"));
    if (gps.time.second() < 10) SerialBT.print(F("0"));
    SerialBT.print(gps.time.second());
    SerialBT.print(F("."));
    if (gps.time.centisecond() < 10) SerialBT.print(F("0"));
    SerialBT.print(gps.time.centisecond());
  } else {
    SerialBT.print(F("INVALID"));
  }

  SerialBT.println();
}

// Output of the result in the Arduino console
void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void sendLocationOverBluetooth() {
  String locationString;
  if (gps.location.isValid()) {
    locationString = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
  } else {
    locationString = "INVALID";
  }

  SerialBT.println("Location: " + locationString);
}