#define SS_PIN D4
#define RST_PIN D3
int ledPinIN = D0;
int ledPinDenied = D2;
int ledPinOut = D1;

#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>


#define FIREBASE_HOST " "
#define FIREBASE_AUTH " "

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;

const char *wifiName = "D03CDialog1-7003";
const char *wifiPass = "30076757";

void setup()
{
  pinMode(ledPinIN, OUTPUT);
  pinMode(ledPinDenied, OUTPUT);
  pinMode(ledPinOut, OUTPUT);
  Serial.begin(9600);   // Initiate a serial communication
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

}
void loop()
{

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
    root["inOrOut"] = "IN";                       //Put In and Out Values
    root.printTo(webPage);                        //Store JSON in String variable
    Serial.print(webPage);

    HTTPClient http;    //Declare object of class HTTPClient

    //Post Data
    String postData;
    postData = webPage;
    http.begin(" ");                 //Specify request destination
    http.addHeader("Content-Type", "application/json");    //Specify content-type header
    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();   //Close connection

    delay(1000);  //Post Data at every 3 seconds

    digitalWrite(ledPinIN, HIGH);
    delay(1000);
    digitalWrite(ledPinIN, LOW);
    Serial.println(" Access Granted ");
    Serial.println(" Welcome Mr.Circuit ");
    Serial.println();
    //statuss = 1;
  }

  else {
    Serial.println(" Access Denied ");
    digitalWrite(ledPinDenied, HIGH);
    delay(1000);
    digitalWrite(ledPinDenied, LOW);
  }
}
