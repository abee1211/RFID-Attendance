#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SPI.h>
#include <MFRC522.h>
#define DEBUG_ON
#include <NTPtimeESP.h>
#define DEBUG_ON


NTPtime NTPch("in.pool.ntp.org");
strDateTime dateTime;
// Set these to run example.
#define FIREBASE_HOST "helloworld-d01cb.firebaseio.com"
#define FIREBASE_AUTH "fAUr486VN87WaYOxzcdASbcVpKWoVHxLyllVYUM4"
#define WIFI_SSID "Realme"
#define WIFI_PASSWORD "12341234"
constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 4;
char *ssid      = "Realme";               // Set you WiFi SSID
char *password  = "12341234";


MFRC522 mfrc522(SS_PIN, RST_PIN);
void setup() {
  Serial.begin(115200);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Serial.println("Place your card");
  SPI.begin();
  mfrc522.PCD_Init();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;
void loop() {
byte block;
byte len; 
String date_time;
  MFRC522::StatusCode status;
  
  if ( mfrc522.PICC_IsNewCardPresent()){
    if ( mfrc522.PICC_ReadCardSerial()){
      long code=0;
      for (byte i = 0; i < mfrc522.uid.size; i++){
        code=((code+mfrc522.uid.uidByte[i])*10);
      }
      Serial.print("Card ID: ");
      Serial.println(code);
 
      dateTime = NTPch.getNTPtime(+5.50, 1);
        //get current timestamp
    if(dateTime.valid){
    NTPch.printDateTime(dateTime);
    byte actualHour = dateTime.hour;
    String hour = String(actualHour);
    byte actualMinute = dateTime.minute;
    String minute = String(actualMinute);
    byte actualsecond = dateTime.second;
    String sec = String(actualsecond);
    int actualyear = dateTime.year;
    String year = String(actualyear);
    byte actualMonth = dateTime.month;
    String month = String(actualMonth);
    byte actualday =dateTime.day;
    String day = String(actualday);
    date_time = year +"-"+ month +"-"+ day +"  "+ hour+":"+ minute+":"+ sec;
    Serial.print("Time : ");
    Serial.println(date_time);
      }
  if (date_time == ""){
    Serial.println();
    Serial.println("Place your card again to register cuurent time:");
    }
  else{
  Firebase.pushString("/rfid/Attendance", "Present");
  if (Firebase.failed()) {
      Serial.print("pushing /Attendance failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(500);

  Firebase.pushInt("/rfid/Card_id", code);
  if (Firebase.failed()) {
      Serial.print("pushing /Card_id failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(500);
  
 Firebase.pushString("/rfid/Date_time", date_time);
  if (Firebase.failed()) {
      Serial.print("pushing /Date_time failed:");
      Serial.println(Firebase.error());  
      return;
  }

  Serial.print("pushed: /logs/");
//  Serial.println(name);
  Serial.println("Place your card");
  delay(500);
    }
  }
}
}
