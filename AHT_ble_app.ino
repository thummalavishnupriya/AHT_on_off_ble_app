//Vehicle on/off, and boot trigger 300mS, with fb to mobile phone
//Temperature sensor data
#include "BluetoothSerial.h"
#define DELAY 300//Delay for turning the button off
BluetoothSerial ESP_BT; //Class name

int led_pin_1 = 13;
int led_pin_2 = 12;

//parameters for bluetooth interface
int incoming;
//for calculating time using millis() function and turn off the boot
unsigned long now;
unsigned long time_boot;

//AHT_read
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
int x;
int y;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  //Name of the bluetooth interface will showup on phone
  ESP_BT.begin("ESP32_Control");
  pinMode(led_pin_1, OUTPUT);
  pinMode(led_pin_2, OUTPUT);
  digitalWrite(led_pin_1, HIGH);
  digitalWrite(led_pin_2, HIGH);
  Serial.println("Adafruit AHT10/AHT20 demo!");
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
}

void buttons() {
  // put your main code here, to run repeatedly:
  digitalWrite(led_pin_2, HIGH);
  now = millis();
  if(digitalRead(led_pin_2) and now > time_boot + DELAY){
    digitalWrite(led_pin_2, 1);
    ESP_BT.write(20);//Writing that boot is off to bluetooth app!
    Serial.print("Boot timeout happend----");
  }
  
  if(ESP_BT.available())
  {
    incoming = ESP_BT.read();//Just reading what we had received
    //Seperating button id's from values ->button id is 10,20 etc value is 1 or 0
    int button = floor(incoming/10);//To detect which button is toggled
    int value = incoming%10;//To detect whether logic is high or low
    switch(button) {
      case 1:
        value =!value;
        Serial.print("Button 1:"); Serial.println(value);
        digitalWrite(led_pin_1, value);
        break;
      case 2:
        value =!value;
        Serial.print("Button 2:"); Serial.println(value);
        digitalWrite(led_pin_2, value);
        if(value == 1)  {
          time_boot = now;
        }
        break;    
    }
  }  
}

void values() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); 
  x = temp.temperature;
  Serial.print(x); 
  int T1 = ESP_BT.write(x);
  Serial.println(" degrees C");
  Serial.print("Humidity: "); 
  y = humidity.relative_humidity;
  Serial.print(y);
  int H1 = ESP_BT.write(y);
  Serial.println("% rH");
  delay(100);  
}
void loop() {
  buttons();
  values();
}
