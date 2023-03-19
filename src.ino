#include <SoftwareSerial.h>

#define DEBUG true
int x, count = 0;

//Traffic Light
int red_a = 13;
int yellow_a = 12;
int green_a = 11;

//Crossing
int red_b = 10;
int green_b = 9;

//Buzzer
int buzzer = 8;

//Button
int push_btn = 7;

String apiKey = "api_key";

// UNO's D10 connected to ESP's TX
// UNO's D11 connected to ESP's RX via resistor network
SoftwareSerial ESP01(2, 3); // RX, TX

void setup() {                
 //Traffic Light
  pinMode(red_a, OUTPUT);
  pinMode(yellow_a, OUTPUT);
  pinMode(green_a, OUTPUT);

  //Crossing
  pinMode(red_b, OUTPUT);
  pinMode(green_b, OUTPUT);

  //Buzzer
  pinMode(buzzer, OUTPUT);

  //Button
  pinMode(push_btn, INPUT);
  
  Serial.begin(9600); 
  while (!Serial){
  }
  Serial.println("Starting...");
  ESP01.begin(9600);
}

void loop() {
  // Reset ESP8266, put it into mode 1 i.e. STA only, make it join hotspot / AP, 
  // establish single connection
  Serial.println();
  sendData("AT+RST\r\n",2000,DEBUG);
  sendData("AT+CWMODE=1\r\n",2000,DEBUG);
  sendData("AT+CWJAP=\"IOT-Elective705-1\",\"pass1234\"\r\n",4000,DEBUG);  
   // ********************** Change these!
  sendData("AT+CIPMUX=0\r\n",2000,DEBUG);

int btnState = digitalRead(push_btn);

do {
  // When traffic light is GREEN
  digitalWrite(green_a, HIGH);
  digitalWrite(yellow_a, LOW);
  digitalWrite(red_a, LOW);
  //Crossing
  digitalWrite(red_b, HIGH);
  digitalWrite(green_b, LOW);
  btnState = digitalRead(push_btn);
} while (btnState == LOW);

if (btnState == HIGH) {
  count++;
  Serial.println("=========[COUNTER]========");
  Serial.println(count);
  Serial.println("==========================");
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  
  delay (5000);
  
  // When traffic light is YELLOW
  digitalWrite(green_a, LOW);
  digitalWrite(yellow_a, HIGH);
  digitalWrite(red_a, LOW);
  //Crossing
  digitalWrite(red_b, HIGH);
  digitalWrite(green_b, LOW);
  
  delay(3000);
  
  // When traffic light is RED
  digitalWrite(green_a, LOW);
  digitalWrite(yellow_a, LOW);
  digitalWrite(red_a, HIGH);
  //Crossing
  digitalWrite(red_b, LOW);
  digitalWrite(green_b, HIGH);
  
  delay(100);

  } 

  // Make TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // Thingspeak.com's IP address  
  // ********************** Change this!
  cmd += "\",80\r\n";
  sendData(cmd,2000,DEBUG);

  // Prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += count;
  getStr += "\r\n";
  
  // Send data length & GET string
  ESP01.print("AT+CIPSEND=");
  ESP01.println (getStr.length());
  Serial.print("AT+CIPSEND=");
  Serial.println (getStr.length());  
  delay(500);
  if( ESP01.find( ">" ) )
  {
    Serial.print(">");
    sendData(getStr,2000,DEBUG);
  }

  // Close connection, wait a while before repeating...
  sendData("AT+CIPCLOSE",16000,DEBUG); // thingspeak needs 15 sec delay between updates
}

String sendData(String command, const int timeout, boolean debug)
{
    String response="";
    ESP01.print(command);
    long int time = millis();
    while ( (time+timeout) > millis() ) {
    
      while (ESP01.available()) {  
    
      char c = ESP01.read();
      response+=c;
    }
  }
  if (debug) {
    Serial.print("Response: " + response);
  }
  return(response);
}