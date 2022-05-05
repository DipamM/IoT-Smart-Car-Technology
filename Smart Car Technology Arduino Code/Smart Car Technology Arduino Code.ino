#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <DHT.h>
#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
//WiFiClient client;
WiFiServer server(80);

const char* ssid = "Modi House";
const char* password = "prcomputers";

#define FIREBASE_HOST  "https://smart-car-tech-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "6rFIaBUBb8bs1vS1BD9BJd9zpykB6aTifixRktIK"

#define SS_PIN 5
#define RST_PIN 22
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String content = "";

#define rainDigital 34
#define rainAnalog 35

#define DHTPIN 16
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int trigPin = 4;
const int buzzPin = 12;
const int echoPin = 13;

unsigned long pre_time = 0;
unsigned long cur_time = 0;
int sec = 0;

long duration;
float distance;

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);

  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(400);
    Serial.print("..");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println();
  Serial.println("NodeMCU Local IP is : ");
  Serial.println((WiFi.localIP()));
  Serial.println("");
  Serial.println("Put your card to the reader for scanning ...");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  dht.begin();
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzPin, OUTPUT);

  pinMode(rainDigital, INPUT);
//  pinMode(led, OUTPUT);  //initialize the LED pin as an output
//  pinMode(LDR, INPUT); 
 // pinMode(switchPin, INPUT);
 //  myservo.attach(2);  
}


void Display(String str1, String str2)
{

  lcd.setCursor(0, 0);
  lcd.print(str1);
 // Serial.println(str1);
  lcd.setCursor(0, 1);
  lcd.print(str2);
 // Serial.println(str2);
}


void loop()
{
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
  Serial.println("UID tag :");

  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
//  Serial.println(content);
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  // enter your own card number after copying it from serial monitor

  if (content.substring(1) == "69 57 A8 A2")
  {
    Serial.println("Authorized access");
    tone(buzzPin, 2000);
    delay(100);
    noTone(buzzPin);
    delay(50);
    tone(buzzPin, 2000);
    delay(100);
    noTone(buzzPin);
    delay(500);
//    if (Firebase.failed())
//
//    {
//      Serial.println("Firebase Failed");
//      Serial.println(Firebase.error());
//      return;
//    }    

    lcd.begin(); // initialize the lcd
    lcd.backlight();  // backlight ON

    Display("  IOT Project   ", "      ON        ");
    delay(2000);
    Firebase.setString("rfid", "True");    
    Display("   Smart Car    ", "   Technology   ");
    delay(2000);
    Display("      Dipam     ", "      Modi      ");
    delay(2000);
    Display("     Engine     ", "     Start      ");
    delay(1000);    
    Firebase.setString("Engine", "ON");

    while (true)
    {
     fuellvl();     
     
     cur_time = millis();
    if ((cur_time - pre_time) >= 1000)
    {
      sec++;
      pre_time = cur_time;
      delay(400);
      Serial.println();
      Serial.println("Kilometers:" + String(sec));
      
      
    }  
     
  //  float h = dht.readHumidity();  // read humidity and store in h float type variable
    float t = dht.readTemperature(); // Read temperature in celcious
  //  float f = dht.readTemperature(true); //  temperature in Fahreinheit
  Firebase.setInt("Km", sec);
      Firebase.setInt("Cabin Temp", t);
    Serial.println("Cabin Temperature: " + String(t) );
    delay(100);
    Serial.println();
    
    Display("Drive Mode   " + String(sec) , "Cabin Temp  " + String(t));
    delay(1000);
      
      
      RainFunc();
    } 
  }

  else   {
    Serial.println(" Access denied");
    delay(100);
    tone(buzzPin, 1500);
    delay(500);
    noTone(buzzPin);
    delay(100);
    tone(buzzPin, 1500);
    delay(500);
    noTone(buzzPin);
    delay(100);
    tone(buzzPin, 1500);
    delay(500);
    noTone(buzzPin);
    delay(100);
    
    Firebase.setString("rfid", "False");
    Firebase.setString("Engine", "OFF");
  }
}

void fuellvl()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
   distance = 0.034 * (duration / 2);

  //  Firebase.setFloat("fuel", distance);
    //Serial.println("Fuel Level: " + String(distance));

    if (distance >= 7 && distance <= 9)
    {
     tone(buzzPin, 1500);
    delay(500);
    noTone(buzzPin);
    delay(100);
    tone(buzzPin, 1500);
    delay(500);
    noTone(buzzPin);
    delay(100);
    tone(buzzPin, 1500);
    delay(500);
    noTone(buzzPin);
    delay(1000);

    float dist= 100-(distance*10);
      Firebase.setFloat("fuel", dist);
      Serial.println("Fuel: LOW");
      Serial.println();
      
      Serial.println("Fuel Level: " +String(dist) + "%");
     // delay(400);
    }
    else if (distance <= 4 && distance >= 2)
    {
      float dist1= 100-(distance*10);
      Firebase.setFloat("fuel", dist1);
      digitalWrite(buzzPin, LOW);    
      Serial.println();
      Serial.println("Fuel: HALF");
      
      Serial.println("Fuel Level: " +String(dist1) + "%");
      delay(300);
    }
    else if (distance <= 1.4)
    {
      float dist2= 100-(distance*10);
      Firebase.setFloat("fuel", dist2);
      digitalWrite(buzzPin, LOW);
      delay(300); 
      Serial.println();
      Serial.println("Fuel: Full");
      
      Serial.println("Fuel Level: " +String(dist2) + "%");         
        
    }

    else if (distance >= 10)
    {
      Serial.println();
      Serial.println("Fuel: Empty");
      Serial.println("Fuel Level: 0% ");  
      digitalWrite(buzzPin, LOW);
    }
}

void RainFunc()
{

  int rainAnalogVal = analogRead(rainAnalog);
  int rainDigitalVal = digitalRead(rainDigital);

  Firebase.setInt("rainsensor", rainAnalogVal);
  
  if (rainAnalogVal > 800)
  {
    // myservo.write(180);
  //  Display("Drive Mode   " + String(sec) , "Cabin Temp  " + String(temp1));
    delay(1000);

  }

  else if (rainAnalogVal <= 800 && rainAnalogVal > 600)
  {
    Serial.println();
    Serial.println("Rain Sensor Value: " + String(rainAnalogVal));
    Display("  IT IS RAINING ", "  AMOUNT: LOW   ");
    delay(1000);
  }

  else if (rainAnalogVal <= 600 && rainAnalogVal > 460) 
    {
    Serial.println();
    Serial.println("Rain Sensor Value: " + String(rainAnalogVal));
    Display("  IT IS RAINING ", " AMOUNT: MEDIUM ");
    delay(1000);
  }

  else if (rainAnalogVal < 460) 
  {
    Serial.println();
    Serial.println("Rain Sensor Value: " + String(rainAnalogVal));
    Display("  IT IS RAINING ", "  AMOUNT: HIGH  ");
  }
  delay(100);
}
