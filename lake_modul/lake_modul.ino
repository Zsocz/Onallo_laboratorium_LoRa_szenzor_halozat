#include <Adafruit_VEML6075.h>
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <SPI.h>
#include <LoRa.h>

// Distance measurement defines & variables
#define between_measurements 1
#define after_measurements 10 //50 sec
#define trig 13
#define echo 36

double distance;
long duration;
String water_level;

//DS18B20 remperature sensor
#define ONE_WIRE_BUS 25 
// Setup a oneWire instance to communicate with any OneWire devices  
OneWire oneWire(ONE_WIRE_BUS); 
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);

float water_temperature;

//Water pump sound detection
int soundDetectedPin = 34;
boolean pump_working = false;

Adafruit_VEML6075 UV = Adafruit_VEML6075();
float UVA;
float UVB;
float UV_index;

#define SS 18
#define RST 14
#define DI0 26

void setup()
{
  Serial.begin(115200);
  delay(50);
  
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  pinMode (soundDetectedPin, INPUT) ;
  // Initialize temperature sensor
  
  pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
  DS18B20.begin();

  if (! UV.begin()) {
    Serial.println("Failed to communicate with VEML6075 sensor, check wiring?");
  }
    
  Wire.begin();
 
  // Set trig pins as output and echo pins as input
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT_PULLUP);
  delay(500);
}

double measure_distance(int trigPin, int echoPin){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  double distance= duration*0.034/2;
  return distance;
}

void readDS18B20(){
  DS18B20.requestTemperatures(); 
  water_temperature = DS18B20.getTempCByIndex(0);
  Serial.print("  Water_temp: " + String(water_temperature) +"°C"); 
 
}

void readUV(){
  UVA=UV.readUVA();
  UVB=UV.readUVB();
  UV_index=UV.readUVI();
}


void calcWaterLevel(){
  if(distance < 45){
    water_level="High";
  }
  else if(distance > 55){
    water_level="Low";
  }
    else if(distance<=55 && distance>=45){
      water_level="Normal";
    }

  Serial.print("  Water level: ");
  Serial.print(water_level);
}


boolean sound_detection(){
  
   int soundDetectedVal = digitalRead (soundDetectedPin) ; // read the sound alarm time
   int sound_positive=0;
  
  if (soundDetectedVal == 1) {
    sound_positive++;
    delay(1000);
    soundDetectedVal = digitalRead (soundDetectedPin) ;
      if (soundDetectedVal == 1) {
      sound_positive++;
      delay(1000);
      soundDetectedVal = digitalRead (soundDetectedPin) ;
       if (soundDetectedVal == 1) {
          sound_positive++;
          delay(1000);
          soundDetectedVal = digitalRead (soundDetectedPin) ;
           if (soundDetectedVal == 1) {
            sound_positive++;
            delay(1000);
            soundDetectedVal = digitalRead (soundDetectedPin) ;
            if (soundDetectedVal == 1) {
              sound_positive++;
            }
          }
        }
      }
  }
    
    if(sound_positive>=3){
      Serial.println("  Pump: ON");
      pump_working = true;
    }
  
 
  if(!pump_working){
    Serial.println("  Pump: OFF");
  }
  
}


void sendData(){
  
  LoRa.beginPacket();
  LoRa.print(2);
  LoRa.print("&");
  LoRa.print(water_temperature);
  LoRa.print("&");
  LoRa.print(water_level);
  LoRa.print("&");
  LoRa.print(UVA);
  LoRa.print("&");
  LoRa.print(UVB);
  LoRa.print("&");
  LoRa.print(UV_index);
  LoRa.endPacket();

  Serial.println("\nData sent on LoRa.");
}

void loop()
{

  distance = measure_distance(trig, echo);
  Serial.print("Water level: ");
  Serial.print(distance);
  Serial.print(" cm");
  calcWaterLevel();

  readDS18B20();

  readUV();
  String uv_str="  UVA: "+String(UVA) + "  UVB: "+String(UVB) + "  UV index: " + String(UV_index);
  Serial.print(uv_str);

  sendData();
  
  Serial.println();
  delay(2000);
}
 
