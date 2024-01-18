#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);

  // Initialize the SoftwareSerial for GPS communication
  ss.begin(GPSBaud);

  Serial2.begin(9600);
  delay(3000);
  test_sim800_module();
  send_SMS();
}

void loop() {
  updateSerial();

  // Check if there is GPS data available
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      // GPS data is available, display it
      displayInfo();
    }
  }

  // Check if GPS data is not available after 5 seconds
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
  }
}

void test_sim800_module()
{
  Serial2.println("AT");
  updateSerial();
  Serial.println();
  Serial2.println("AT+CSQ");
  updateSerial();
  Serial2.println("AT+CCID");
  updateSerial();
  Serial2.println("AT+CREG?");
  updateSerial();
  Serial2.println("ATI");
  updateSerial();
  Serial2.println("AT+CBC");
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    Serial2.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (Serial2.available())
  {
    Serial.write(Serial2.read()); // Forward what Software Serial received to Serial Port
  }
}

void send_SMS()
{
  Serial2.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  Serial2.println("AT+CMGS=\"+380730332911\""); // phone number to sms
  updateSerial();
  Serial2.print("Circuit Digest"); // text content
  updateSerial();
  Serial.println();
  Serial.println("Message Sent");
  Serial2.write(26);
}

void send_GPS_SMS() {
  // Check if GPS data is valid
  if (gps.location.isValid()) {
    // Format GPS data into a string
    String message = "Location: " + String(gps.location.lat(), 6) + ", " + String(gps.location.lng(), 6);

    // Send GPS data via SMS
    send_SMS(message);
  } else {
    Serial.println(F("Invalid GPS data"));
  }
}

void displayInfo() {
  // Print the GPS location
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  // Print the date
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

  // Print the time
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