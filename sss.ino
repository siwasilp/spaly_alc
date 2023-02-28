#include <WiFi.h>
#include <Wire.h>

// Wifi Setup
const char* ssid = "00000000";
const char* password = "00000000";

// IFTTT URL
const char* resource = "/trigger/pump_0/with/key/behqCXPTbp1UtkOAptS3Ue";
const char* server = "maker.ifttt.com";



#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);   //Module IIC/I2C Interface บางรุ่นอาจจะใช้ 0x3f
#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const int pingPin = 18;
int inPin = 5;

int wat = 32;
int r_1 = 25;
int r_2 = 26;
int alm = 4;
int x, y;
long duration, cm;


void setup()
{
  pinMode(alm, OUTPUT);
  pinMode(r_1, OUTPUT);
  pinMode(r_2, OUTPUT);
  pinMode(wat, INPUT);
  Serial.begin(9600);
  digitalWrite(r_1, HIGH);
  digitalWrite(r_2, HIGH);
  digitalWrite(alm, LOW);
  mlx.begin();
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Smart Spray Alcoho ");
  lcd.setCursor(0, 1);
  lcd.print(" ssid : "); lcd.print(String(ssid));
  lcd.setCursor(0, 2);
  lcd.print(" pass : "); lcd.print(String(password));
  lcd.setCursor(0, 3);
  lcd.print(" Wifi Connect       ");
  initWifi();
  delay(5000);
  lcd.clear();

}

float xxx = 0;
int yyy = 0;
String zzz ;
String aaa ;

void loop()
{
  xxx = temp();
  yyy = dist();
  zzz = water();
  aaa = "Online";

  lcd.setCursor(0, 0);
  lcd.print("Dist  : "); lcd.print(yyy); lcd.print(" cm ");
  lcd.setCursor(0, 1);
  lcd.print("Temp  : "); lcd.print(xxx); lcd.print(" C  ");
  lcd.setCursor(0, 2);
  lcd.print("Alco  : "); lcd.print(zzz);
  lcd.setCursor(0, 3);
  lcd.print("Staus : "); lcd.print(aaa); lcd.print("  ");

  if (yyy < 5) {
    if (xxx > 38) {

      lcd.setCursor(8, 3);
      lcd.print("TEMP OVER ! ");
      digitalWrite(alm, HIGH);
      delay(500);
      digitalWrite(alm, LOW);
      delay(500);
      digitalWrite(alm, HIGH);
      delay(500);
      digitalWrite(alm, LOW);
      delay(500);
      digitalWrite(alm, HIGH);
      delay(500);
      digitalWrite(alm, LOW);
      delay(500);
      lcd.setCursor(8, 3);
      lcd.print("            ");
      makeIFTTTy();
    }
    lcd.setCursor(8, 3);
    lcd.print("Spray!!!");
    digitalWrite(r_1, HIGH);
    digitalWrite(r_2, LOW);
    delay(1000);
    digitalWrite(r_1, LOW);
    digitalWrite(r_2, HIGH);
    delay(1000);
    digitalWrite(r_1, HIGH);
    digitalWrite(r_2, LOW);
    delay(1000);
    digitalWrite(r_1, LOW);
    digitalWrite(r_2, HIGH);
    delay(1000);
    lcd.setCursor(8, 3);
    lcd.print("OK...!!!");
    digitalWrite(r_1, HIGH);
    digitalWrite(r_2, LOW);
    delay(1000);
    digitalWrite(r_1, LOW);
    digitalWrite(r_2, HIGH);
    delay(1000);
  }
}

String water() {
  int val = analogRead(wat);
  if (val < 500) {
    Serial.println("No Alcohol.");
    makeIFTTTx();
    return "No Alcohol.";
  }
  if (val > 500) {
    Serial.println("Detec Alc.");
    return "Detec Alc. ";
  }
}

float temp() {
  Serial.print("Ambient = ");
  Serial.print(mlx.readAmbientTempC());
  Serial.print("*C\tObject = ");
  Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = ");
  Serial.print(mlx.readAmbientTempF());
  Serial.print("*F\tObject = ");
  Serial.print(mlx.readObjectTempF()); Serial.println("*F");
  Serial.println();
  return mlx.readObjectTempC();
}

int dist()
{
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  cm = microsecondsToCentimeters(duration);
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(100);
  return cm;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}


// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  lcd.setCursor(13, 3);
  while (WiFi.status() != WL_CONNECTED && (timeout > 0)) {
    delay(250);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect, going back to sleep");
    lcd.setCursor(13, 3);
    lcd.print("Failed");
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void makeIFTTTx() {
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect…");
  }
  Serial.print("Request resource: ");
  Serial.println(resource);
  String jsonObject = String("{\"value1\":\"No Alcohol.""\",\"value2\":\"" + String(xxx) + " C \",\"value3\":\"" +  String(yyy) + " Cm \"}"); //สร้างชุดสริง json
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);
  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    Serial.println("No response…");
  }
  while (client.available()) {
    Serial.write(client.read());
  }
  Serial.println("\nclosing connection");
  client.stop();
}

void makeIFTTTy() {
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect…");
  }
  Serial.print("Request resource: ");
  Serial.println(resource);
  String jsonObject = String("{\"value1\":\"OVER TEMP !!! ""\",\"value2\":\"" + String(xxx) + " C \",\"value3\":\"" +  String(yyy) + " Cm \"}"); //สร้างชุดสริง json
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);
  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    Serial.println("No response…");
  }
  while (client.available()) {
    Serial.write(client.read());
  }
  Serial.println("\nclosing connection");
  client.stop();
}
