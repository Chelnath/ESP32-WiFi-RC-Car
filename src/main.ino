#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>

int in1 = 18;
int in2 = 19;
int in3 = 21;
int in4 = 22;

const int wifiLedPin = 4;

String command;
int SPEED = 122;
int speed_Coeff = 3;

WebServer server(80);

unsigned long previousMillis = 0;

String sta_ssid = "";
String sta_password = "";

int pwmChannel1 = 0;
int pwmChannel2 = 1;
int pwmFreq = 1000;
int pwmResolution = 10;

void setup() {
  Serial.begin(115200);

  pinMode(wifiLedPin, OUTPUT);
  digitalWrite(wifiLedPin, HIGH);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  ledcSetup(pwmChannel1, pwmFreq, pwmResolution);
  ledcSetup(pwmChannel2, pwmFreq, pwmResolution);

  ledcAttachPin(in1, pwmChannel1);
  ledcAttachPin(in3, pwmChannel2);

  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis <= 10000) {
    delay(500);
    Serial.print(".");
    currentMillis = millis();
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(wifiLedPin, LOW);
    delay(3000);
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32_Car");
    digitalWrite(wifiLedPin, HIGH);
    delay(3000);
  }

  server.on("/", HTTP_handleRoot);
  server.onNotFound(HTTP_handleRoot);
  server.begin();

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  command = server.arg("State");
  if (command == "F") Forward();
  else if (command == "B") Backward();
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "G") ForwardLeft();
  else if (command == "H") BackwardLeft();
  else if (command == "I") ForwardRight();
  else if (command == "J") BackwardRight();
  else if (command == "S") Stop();
  else if (command == "0") SPEED = 60;
  else if (command == "1") SPEED = 70;
  else if (command == "2") SPEED = 81;
  else if (command == "3") SPEED = 95;
  else if (command == "4") SPEED = 105;
  else if (command == "5") SPEED = 122;
  else if (command == "6") SPEED = 150;
  else if (command == "7") SPEED = 196;
  else if (command == "8") SPEED = 272;
  else if (command == "9") SPEED = 400;
  else if (command == "q") SPEED = 1023;
}

void HTTP_handleRoot() {
  server.send(200, "text/html", "");
  if (server.hasArg("State")) Serial.println(server.arg("State"));
}

void Forward() {
  ledcWrite(pwmChannel1, SPEED);
  digitalWrite(in2, LOW);
  ledcWrite(pwmChannel2, SPEED);
  digitalWrite(in4, LOW);
}

void Backward() {
  digitalWrite(in1, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in4, HIGH);
}

void TurnRight() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  ledcWrite(pwmChannel2, SPEED);
  digitalWrite(in4, LOW);
}

void TurnLeft() {
  ledcWrite(pwmChannel1, SPEED);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void ForwardLeft() {
  ledcWrite(pwmChannel1, SPEED);
  digitalWrite(in2, LOW);
  ledcWrite(pwmChannel2, SPEED / speed_Coeff);
  digitalWrite(in4, LOW);
}

void BackwardLeft() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void ForwardRight() {
  ledcWrite(pwmChannel1, SPEED / speed_Coeff);
  digitalWrite(in2, LOW);
  ledcWrite(pwmChannel2, SPEED);
  digitalWrite(in4, LOW);
}

void BackwardRight() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void Stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
