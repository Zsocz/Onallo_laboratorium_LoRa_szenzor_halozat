
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "RTClib.h"
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    868E6


SSD1306 display(0x3c, 21, 22); 

//MySQL server
IPAddress server_addr(xxx,xxx,xxx,xxx);  // IP of the MySQL *server* here
char sql_user[] = "xxxx";   // MySQL user login username
char sql_pw[] = "xxxx";      // MySQL user login password

//Wifi
const char* ssid     = "xxxx";
const char* wifi_pw =  "xxxx";

WiFiClient client;
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

boolean Connected = false;

RTC_DS3231 rtc;
String Time;

String packet ;
String modul1[4];
String modul2[5];
String modul1_inline;
String modul2_inline;

int received_1=0;
int received_2=0;

char SQL_update_current[]= "UPDATE nagyzsol_onlab.current_sensor_data SET air_temp'%f', humidity='%f', soil_moisture='%d', lux='%d', water_temp='%f', water level= '%s'UVA='%f', UVB='%f', UV_index='%f'";

void setup() {
  
  Serial.begin(115200);
  delay(50);

  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high、
  
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  LoRa.receive();
  Serial.println("Init ok");
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("Cock lost power!");
    Time="NULL";
  }

  WiFi.begin(ssid, wifi_pw);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.println("Waiting for packets");
  delay(1500);
}

bool sendSQL(){

  String insert_query = "INSERT INTO nagyzsol_onlab.sensor_data(air_temp, humidity, soil_moisture, lux, water_temp, water_level, UVA, UVB, UV_index) VALUES (" 
                        + modul1[0] +","
                        + modul1[1] +","
                        + modul1[2] +","
                        + modul1[3] +","
                        + modul2[0] +","
                        + modul2[1] +","
                        + modul2[2] +","
                        + modul2[3] +","
                        + modul2[4] +");";

  char insert[insert_query.length()];   
  insert_query.toCharArray(insert, insert_query.length()) ;

  String update_query = "UPDATE nagyzsol_onlab.current_sensor_data SET air_temp=" + modul1[0] 
                        + ",humidity="      + modul1[1]
                        + ",soil_moisture=" + modul1[2]
                        + ",lux="           + modul1[3] 
                        + ",water_temp="    + modul2[0] 
                        + ",water_level="   + modul2[1] 
                        + ",UVA="           + modul2[2] 
                        + ",UVB="           + modul2[3] 
                        + ",UV_index="      + modul2[4]  
                        +";" ;

  char updater[update_query.length()];   
  update_query.toCharArray(updater, update_query.length()) ;

  /*
    // Print SQL query to serial
    for(int i=0 ; i<insert_query.length(); i++){
    Serial.print(insert_query[i]);
  }
  Serial.println();
  */

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, sql_user, sql_pw))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
  
  if (conn.connected()){
    cursor->execute(insert);
    cursor->execute(updater);
    Serial.println("Data sent.");
    Serial.println();
    return true;
  }
  else{
    Serial.println("Error:SQL connection lost.");
    Serial.println();
    return false;
  }
}

void displayData(){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0 , 2  , 128, String("1: ")+ modul1_inline);
  display.drawStringMaxWidth(0 , 36 , 128, String("2: ")+ modul2_inline);
  display.display();
}

void processReceivedData(int packetSize) {
  
  packet ="";
  char ID = (char)LoRa.read();
  LoRa.read();

  if(ID=='1'){
    received_1=true;
    modul1_inline="";
    for(int i=0; i< 4; i++){
      modul1[i]="";
    }
  }

  if(ID=='2'){
    received_2=true;
    modul2_inline=""; 
    for(int i=0; i< 5; i++){
      modul2[i]="";
    }
  }
  
  int k = 0;
  char tmp;

  for (int i = 1; i < packetSize-1; i++) { 
    tmp = (char)LoRa.read();
    
    if(ID=='1' && tmp!='&'){
      modul1[k]+=tmp;
      modul1_inline+=tmp;
    }

    if(ID=='2' && tmp!='&'){
      modul2[k]+=tmp;
      modul2_inline+=tmp;
    }

    if(tmp =='&'){
      
      if(ID=='1'){
        modul1_inline+="  ";
      }
      if(ID=='2'){
        modul2_inline+="  ";
      }
      k++;
    }

  if(modul2[1].equals("High")){
    modul2[1]="1";
  }
  else if(modul2[1].equals("Normal")){
    modul2[1]="2";
  }
  else if(modul2[1].equals("Low")){
    modul2[1]="3";
  }

    
  }

  if(ID=='1'){
    Serial.print("modul1: ");
    Serial.print(modul1_inline);
  }
  if(ID=='2'){
    Serial.print("modul2: ");
    Serial.print(modul2_inline);
  }
  
  displayData();
}

void readTime(){
  DateTime now = rtc.now();
  Time = String(now.year(), DEC)   + "-" + String(now.month(), DEC) + "-" +
         String(now.day(), DEC)    + " " + String(now.hour(), DEC)  + ":" +
         String(now.minute(), DEC) + ":" + String(now.second(), DEC);
}


void loop() {

  int packetSize = LoRa.parsePacket();

  if (packetSize) { 
    readTime();
    Serial.print(Time);
    Serial.print(" - ");
    processReceivedData(packetSize);
    Serial.println();
  }

  if(received_1 && received_2){
    if(sendSQL()){
      display.clear();
      display.display();
      modul1_inline="";
      modul2_inline="";
    }
    received_1=false;
    received_2=false;
  }
  
  
  
  delay(10);
}
