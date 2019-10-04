#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>

#define SS_PIN D4
#define RST_PIN D3
#define FIREBASE_HOST " "
#define FIREBASE_AUTH " "

int ledPin = D2;
int isObstaclePin1 = D0;
int isObstaclePin2 = D1;
int isObstacle1 = HIGH;
int isObstacle2 = HIGH;
int ledPinOut = D8;

const char *wifiName = "D03CDialog1-7003";
const char *wifiPass = "30076757";
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;
void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(ledPinOut, OUTPUT);
  pinMode(isObstaclePin1, INPUT);
  pinMode(isObstaclePin2, INPUT);
  Serial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  WiFi.begin(wifiName, wifiPass); //Connect to WiFi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("#");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop()
{
  isObstacle1 = digitalRead(isObstaclePin1);
  isObstacle2 = digitalRead(isObstaclePin2);
  if (isObstacle1 == LOW || isObstacle2 == LOW)
  {
    Serial.println("Parked");
    //exit(0);
    digitalWrite(ledPin, HIGH);

  }
  else
  {
    Serial.println("clear");
    digitalWrite(ledPin, LOW);
  }
  delay(200);

  String fireSlot1 = String(isObstacle1);
  String fireSlot2 = String(isObstacle2);


  Firebase.setString(" ", fireSlot1);
  Firebase.setString(" ", fireSlot2);

  String webPage;
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print("UID tag :");
  String content = " ";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  if (content.substring(1) == ("ABB3FA0A")) //change UID of the card that you want to give access
  {
    String webPage;
    StaticJsonBuffer<750> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["rfid"] = content.substring(1);          //Put RFID tag value
    root["inOrOut"] = "OUT";                       //Put In and Out Values
    root.printTo(webPage);                        //Store JSON in String variable
    Serial.print(webPage);

    HTTPClient http;    //Declare object of class HTTPClient

    //Post Data
    String postData;
    postData = webPage;
    http.begin("http://...");                 //Specify request destination
    http.addHeader("Content-Type", "application/json");    //Specify content-type header
    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();   //Close connection

    delay(3000);  //Post Data at every 3 seconds

    digitalWrite(ledPinOut, HIGH);
    delay(1000);
    digitalWrite(ledPinOut, LOW);
    Serial.println(" Exit From ParkMe ");
    Serial.println(" Collect your Bill ");
    Serial.println();
    statuss = 1;
  }
}
