#include <DFRobot_DHT11.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <Keypad.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <homepage.h>

const char* ssid = "Ernesto";
const char* password = "Vodafone12345";

DFRobot_DHT11 DHT;

#define AO_PIN 34     // Arduino's pin connected to AO pin of the MQ2 sensor
#define DHT11_PIN 17  // ESP32 pin GPIO23 connected to DHT11 sensor
#define RELAY_PIN 15  // ESP32 pin GPIO16 that connects to relay to control the 12V buzzer
#define ROW_NUM 4     // four rows
#define COLUMN_NUM 3  // three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};



const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
long duration;
float cms, inches;

rgb_lcd lcd;

byte pin_rows[ROW_NUM] = { 16, 4, 0, 2 };      // GPIO16, GPIO4, GPIO0, GPIO2 connect to the row pins
byte pin_column[COLUMN_NUM] = { 19, 13, 26 };  // GPIO13, GPIO0, GPIO26 connect to the column pins


Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

WebServer server(80);

void setup() {
  lcd.begin(16, 2);

  // serial port begin
  Serial.begin(115200);


WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
Serial.println("");

  // define inputs and outputs
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(DHT11_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);


 // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }


  server.on("/", handleRoot);
  server.on("/temperature", handleTemperature);
  //server.on("/toggleLED", toggleLED);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  lcd.print("Hello Friend! ");
  delay(2000);
  lcd.clear();
  lcd.print("Please choose");
  lcd.setCursor(0, 1);
  lcd.print("any feature:");
  delay(2000);
  lcd.clear();
  lcd.print("1.Burglar alarm.");
  delay(2000);
  lcd.clear();
  lcd.print("2.Temperature ");
  lcd.setCursor(0, 1);
  lcd.print("and humidity");
  delay(2000);
  lcd.clear();
  lcd.print("3.Smoke alarm");
  delay(2000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  delay(100);

  char key = keypad.getKey();
  switch (key) {

    case '1':
      //burglar alarm
      BurglarAlarm();

      break;
    case '2':
      // temp and humidity sensor code goes here
      TempAndHumidity();

      break;
    case '3':
      // Smoke alarm
      SmokeAlarm();

      break;
    default:

      break;
  }
}

void BurglarAlarm() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  cms = (duration / 2) / 29.1;
  inches = (duration / 2) / 74;
  if (cms > 5) {

    lcd.print("No pedestrians ");
    lcd.setCursor(0, 1);
    lcd.print("found.");
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cms);
    Serial.print("cm");
    delay(1000);
  lcd.clear();
  lcd.print("Inches:  ");  // print message at (0, 0)
  lcd.print(inches);
  lcd.setCursor(0, 1);      // move cursor to   (2, 1)
  lcd.print("Cm: ");  // print message at (2, 1)
  lcd.print(cms);
    Serial.println();
    delay(4000);
    lcd.clear();
  }
  if (cms < 10) {
lcd.print("Pedestrians found!");
    delay(1000);
  lcd.clear();
  lcd.print("Inches:  ");  // print message at (0, 0)
  lcd.print(inches);
  lcd.setCursor(0, 1);      // move cursor to   (2, 1)
  lcd.print("Cm: ");  // print message at (2, 1)
  lcd.print(cms);
    Serial.println();
    delay(4000);
    lcd.clear();
  }
  delay(250);
}

void TempAndHumidity() {
  DHT.read(DHT11_PIN);
  lcd.clear();                 // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("Temperature: ");  // print message at (0, 0)
  lcd.print(DHT.temperature,1);
  lcd.setCursor(0, 1);      // move cursor to   (2, 1)
  lcd.print("Humidity: ");  // print message at (2, 1)
  lcd.print(DHT.humidity);
  delay(4000);  // display the above for two seconds
  lcd.clear();
}


String getTemp() {
  DHT.read(DHT11_PIN);
  float temp = DHT.temperature; // Read temperature
  Serial.println("DHT11 Temperature: " + String(temp, 1) + " °C");
  
  // Return as a string with 1 decimal place
  return String(temp, 1);
}

void handleTemperature() {
  server.send(200, "text/plain", getTemp());
}

void SmokeAlarm() {
  int gasValue = analogRead(AO_PIN);
  if (gasValue < 900) {
    lcd.print("Normal amount");
    lcd.setCursor(0, 1);
    lcd.print("of gas detected.");
    delay(500);
    lcd.clear();
    lcd.print("MQ2 sensor AO ");
    lcd.setCursor(0, 1);
    lcd.print("value: ");
    lcd.print(gasValue);
    delay(4000);
    lcd.clear();
  }
  if (gasValue > 900) {
    lcd.print("High amount of ");
    lcd.setCursor(0, 1);
    lcd.print("gas detected!!!");
    delay(2000);
    lcd.clear();
    lcd.print("MQ2 sensor AO ");
    lcd.setCursor(0, 1);
    lcd.print("value: ");
    lcd.print(gasValue);
    digitalWrite(RELAY_PIN, HIGH);
    delay(2000);
    digitalWrite(RELAY_PIN, LOW);
    delay(2000);
    delay(2000);
    lcd.clear();
  }
}

void handleRoot() {
  String message = homepage;
  server.send(200, "text/html", homepage); // Send the HTML page
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

