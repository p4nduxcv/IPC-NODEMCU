#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST " "
#define FIREBASE_AUTH " "


#define WIFI_SSID "D03CDialog1-7003"
#define WIFI_PASSWORD "30076757"

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


void setup() {
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

}

void loop() {

  FirebaseObject jsonObject1 = Firebase.get("/SLOT1");
  FirebaseObject jsonObject2 = Firebase.get("/SLOT2");

  String SLOT_01 = jsonObject1.getString("State");
  String SLOT_02 = jsonObject2.getString("State");

  if (SLOT_01 == "1") {
    lcd.setCursor(0, 0);
    // print message
    lcd.print("SLOT_01 ");
    lcd.print("Free");
    delay(5000);
    lcd.clear();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("SLOT_01 ");
    lcd.print("Busy");
    delay(2000);
    lcd.clear();
  }

  if ((SLOT_02 == "1")) {
    lcd.setCursor(0, 1);
    lcd.print("SLOT_02 ");
    lcd.print("Free");
    delay(2000);
    lcd.clear();
  } else {
    lcd.setCursor(0, 1);
    lcd.print("SLOT_02 ");
    lcd.print("Busy");
    delay(2000);
    lcd.clear();
  }
}
