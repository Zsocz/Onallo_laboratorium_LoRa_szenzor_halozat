#include <SparkFunTSL2561.h>
#include <Wire.h>
#include <LoRa.h>
#include "DHT.h"

SFE_TSL2561 light;
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
unsigned int visible, infrared;
double lux;
int max_value= 3480;  //DRY
int min_value= 1230;  //WATER
int soil_moisture;

// DHT Sensor
#define DHTTYPE DHT22
uint8_t DHTPin = 23;
// Initialize DHT sensor
DHT dht(DHTPin, DHTTYPE);   
float temperature;
float humidity;

#define SS 18
#define RST 14
#define DI0 26

void setup() {

  Serial.begin(115200);
  delay(50);

  pinMode(DHTPin, INPUT);
  dht.begin();

  LoRa.setPins(SS, RST, DI0);
  
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 
  light.begin();
  
  // 0-> 1X gain  1->16X gain
  gain = 0;
  
  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration
  unsigned char time = 1;
  light.setTiming(gain,time,ms);
  light.setPowerUp(); 

  delay(500);
}

void readDHT22(){
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}

void read_soil_moisture(){
  int tmp=0;
  int tmp2=0;
  
  for(int i=0; i<10; i++){
    tmp2=analogRead(34);
    if(tmp2 > max_value){
      tmp2=max_value;
    }
    if(tmp2 < min_value){
      tmp2=min_value;
    }
    tmp+= tmp2;
    delay(100);
  }

  tmp = tmp/10;

  float percentage = 1.0f;
  percentage -= (float(tmp) - float(min_value))/(float(max_value) - float(min_value));
  
  soil_moisture = int(percentage*100);
}

void get_lux(){
  
  if (light.getData(visible, infrared)){
     
    // The getLux() function will return 1 if the calculation
    // was successful, or 0 if one or both of the sensors was
    // saturated (too much light).
    boolean valid;
    valid = light.getLux(gain,ms,visible,infrared,lux);
    
    if (!valid)
      Serial.println(" Invalid");
  }
  else{
    byte error = light.getError();
    printError(error);
  }
}

void sendData(){
  
  LoRa.beginPacket();
  LoRa.print(1);
  LoRa.print("&");
  LoRa.print(temperature);
  LoRa.print("&");
  LoRa.print(humidity);
  LoRa.print("&");
  LoRa.print(soil_moisture);
  LoRa.print("&");
  LoRa.print(lux);
  LoRa.endPacket();

  Serial.println("\nData sent on LoRa.");
}

void printError(byte error){ // print I2C errors
  
  Serial.print("I2C error: ");
  Serial.print(error,DEC);
  Serial.print(", ");
  
  switch(error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}

void loop() {

  readDHT22();
  String l1= "Temp: " + String(temperature)+"°C";
  String l2= "Humidity: " + String(humidity)+"%";
  Serial.print(l1+"  "+l2);

  read_soil_moisture();
  Serial.print("  Soil moisture: ");
  Serial.print(soil_moisture);
  Serial.print("%");

  get_lux();
  Serial.print("  Lux: ");
  Serial.print(lux);
  Serial.print(" ( Visible: ");
  Serial.print(visible);
  Serial.print("  Infrared: ");
  Serial.print(infrared);
  Serial.print(" )");

  sendData();
  
  Serial.println();
  delay(500);
}
