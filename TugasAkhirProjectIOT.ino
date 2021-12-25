/* --- Firebase --- */
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
#define FIREBASE_HOST "dht11firebase-63c10-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Ey4uoyIxZTVagcqTKOnndeGIgR17IYm6nFtIOMo6"
#define WIFI_SSID "Silviaaaa"
#define WIFI_PASSWORD "123456789"
FirebaseData firebaseData;

/* --- Blynk --- */
#include <SPI.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
char auth[] = "WPSl49cD_fAhxEEgweMVp1fgchfNyr42";
char ssid[]="Silviaaaa";
char pass[]="123456789";
SimpleTimer timer;
void sendSensor(){
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();
  if (isnan(h) || isnan(t)){
    Serial.println("Error!! Gagal membaca sensor DHT11");
    return;
  }
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
}

/* --- Google Sheet --- */
#include "TRIGGER_WIFI.h"
#include "TRIGGER_GOOGLESHEETS.h"
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht11(DHTPIN, DHTTYPE);
char column_name_in_sheets[ ][20] = {"value1","value2"};
String Sheets_GAS_ID = "AKfycbzj1AXzTIW5gg_YP_hY0WVkXrGFflZUd4iUlHj9O5Kd54ssVwiTfPh_lTSv9XrPblL0";
int No_of_Parameters = 2;

/* --- VOID SETUP dari Firebase, Blynk , dan Google Sheet --- */
void setup() {
  Serial.begin(115200);

  //Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Blynk.begin(auth, ssid, pass);
  dht11.begin();
  timer.setInterval(1000L, sendSensor);
  
  //Blynk
  dht11.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(400);
  }
  Serial.println();
  Serial.print("Connected with IP : ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  //Google Sheet
  while (!Serial);
  dht11.begin();
  WIFI_Connect("Silviaaaa","123456789");
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
}

/* --- VOID LOOP dari Firebase, Blynk , dan Google Sheet --- */
void loop() {
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();
  
  //Firebase
  if (isnan(h) || isnan(t)){
    Serial.println("Error!! Gagal membaca sensor DHT11");
    delay(1000);
    return;
  }
  Serial.print("Kelembaban : ");
  Serial.print(h);
  Serial.println();
  Serial.print("Suhu : ");
  Serial.print(t);
  Serial.println();
    if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/suhu", t)){
    Serial.println("Data Suhu terkirim");
  }else{
    Serial.println("Data Suhu tidak terkirim! ");
    Serial.println(firebaseData.errorReason());
  }
  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/kelembaban", h)){
    Serial.println("Data Kelembaban terkirim");
  }else{
    Serial.println("Data Kelembaban tidak terkirim");
    Serial.println(firebaseData.errorReason());
  }

  //Blynk
  Blynk.run();
  timer.run();
  delay(1000);

  //Google Sheet
  Data_to_Sheets(No_of_Parameters,  t,  h);
  Serial.println();
  delay(5000);                       
}
