/*
  Greenhouse
 modified 28.10.2014
 by Fabian Binder
 */
#include <SPI.h>
#include <WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302)

//Analog PINs
#define tempPin_1 0 // Analog pin 0 for temperature sensor TMP36 
#define humitidyPin_1 1 // Analog pin 1 for humitidy sensor
#define hallPin_1 10

//Digital PINs
#define dhtTempHumiPin 2
#define relayPin_1 8
#define sdCardPin 10
#define pwmAirPin 6


//char ssid[] = "WLAN-L8YNC2";     //  your network SSID (name)
//char pass[] = "3910129555865245";    // your network password

//char ssid[] = "HTC Portable Hotspot 2068";     //  your network SSID (name)
//char pass[] = "Faab2014";    // your network password

boolean wiFiActivated = true;

char ssid[] = "UC-Projects-WiFi";     //  your network SSID (name)
char pass[] = "JF1JJXRL1V3JTB9";    // your network password

// Modi
boolean watering = true; // set to true for auto watering


//Relay Status
boolean relayHigh = false;

// Aktuelle Werte Variablen:
int currentId = 0;
String plantName = "NewPlant";
float currentTemp = 0;
float currentHumiGround = 0;
float currentHumiGroundPercent = 0;
float currentHall = 0;
float currentHallPercent = 0;
float current_dht_humiAir = 0;
float current_dht_temp = 0;

// Schwellenwerte:
int MAX_HUMI_GROUND_PERCENT = 30;
int MAX_HUMI_AIR_PERCENT = 100;
int MAX_TEMP = 30;

int MIN_HUMI_GROUND_PERCENT = 70;
int MIN_HUMI_AIR_PERCENT = 20;
int MIN_TEMP = 10;

String jsonString1 = "";
String jsonString2 = "";
String jsonString3 = "";
String jsonString4 = "";
String jsonString5 = "";
String jsonString6 = "";
String jsonString7 = "";

// Server status flag.
int status = WL_IDLE_STATUS;

// Create WiFi server listening on the given port.
WiFiServer server(80);

DHT dht(dhtTempHumiPin, DHTTYPE);

// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);

  // Wait for serial port to connect. Needed for Leonardo only
  //while (!Serial) { 
  // ; 
  //}


  if(wiFiActivated) {
    if (WiFi.status() == WL_NO_SHIELD) {
      // If no shield, print message and exit setup.
      //Serial.println("WiFi shield not present");
      status = WL_NO_SHIELD;
      return;
    }

    String version = WiFi.firmwareVersion();
    if (version != "1.1.0") {
      //Serial.println("Please upgrade the firmware");
    }

    // Connect to Wifi network.
    while (status != WL_CONNECTED){

      //Serial.print("Connecting to Network named: ");
      //Serial.println(ssid);
      // Connect to WPA/WPA2 network. Update this line if
      // using open or WEP network.
      status = WiFi.begin(ssid, pass);
      // Wait for connection.
      delay(1000);
    }

    // Start the server.
    server.begin();


    // Print WiFi status.
    printWifiStatus();
  }
  // Relay PIN setzen
  pinMode(relayPin_1, OUTPUT);

  // TODO: PIN inversen damit standard LOW ist
  // Da das Relay gleich auf high gesetzt wird beim initalisieren -> ausschalten:
  setRelay();
  // Temperatur und Luftfeuchtigkeit ueber DHT Sensor abfragen
  dht.begin();

  //pinMode( pwmAirPin, OUTPUT);

}

void loop() {
  currentId++;
  getTemperature();
  getHumitidyGround();
  getHallActivity();
  getTempAndHumiAirFromDht();

  /*
  Serial.println("PWM to 50...");
   digitalWrite(pwmAirPin, 50);
   delay(4000);
   Serial.println("PWM to 200...");
   analogWrite(pwmAirPin, 200);
   delay(4000);
   Serial.println("PWM to 0...");
   analogWrite(pwmAirPin, 0);*/
  //Serial.println("-----------------");
  //Serial.println("Temp: " + String(currentTemp) + " C");
  //Serial.println("DHT Temp: " + String(current_dht_temp) + " C");
  //Serial.println("DHT Humi Air: " + String(current_dht_humiAir) + "%");
  //Serial.println("Humi Ground: " + String(currentHumiGroundPercent) + "%");
  //Serial.println("Hall: " + String(currentHallPercent) + "%");

  //Serial.println("-----------------");

  if(wiFiActivated) networkLoop();

  checkNeedWater();


  parseJsonOne();
  parseJsonTwo();
  parseJsonThree();
  parseJsonFour();
  parseJsonFive();
  parseJsonSix();
  parseJsonSeven();

  delay(1000);

}

void parseJsonOne() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["id"] = currentId;
  //root["plantName"] = "Petersilie";
  root["timeStamp"] = currentId;

  char buffer[200];
  root.printTo(buffer, sizeof(buffer));
  String a(buffer);
  Serial.println(a);
  jsonString1 = a;
}

void parseJsonTwo() {
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["currentTemp_1"] = current_dht_temp;
  root["currentTemp_2"] = currentTemp;
  root["currentHumiAir"] = current_dht_humiAir;
  char buffer[80];
  root.printTo(buffer, sizeof(buffer));
  String a(buffer);
  Serial.println(a);
  jsonString2 = a;
}

void parseJsonThree() {
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["currentHumiGround"] = currentHumiGroundPercent;
  root["lightPercent"] = 100.00; //TODO abaendern
  root["airPercent"] = 80.00; //TODO abaendern

  char buffer[80];
  root.printTo(buffer, sizeof(buffer));
  String a(buffer);
  Serial.println(a);
  jsonString3 = a;
}

void parseJsonFour() {
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["lightOn"] = true; //TODO abaendern
  root["airOn"] = true; //TODO abaendern
  root["waterOn"] = false; //TODO abaendern
  char buffer[80];
  root.printTo(buffer, sizeof(buffer));
  String a(buffer);
  Serial.println(a);
  jsonString4 = a;
}

void parseJsonFive() {
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["maxTemp"] = MAX_TEMP;
  root["minTemp"] = MIN_TEMP;
  root["maxHumiAir"] = MAX_HUMI_AIR_PERCENT;

  char buffer[80];
  root.printTo(buffer, sizeof(buffer));
  String a(buffer);
  Serial.println(a);
  jsonString5 = a;
}

void parseJsonSix() {
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["minHumiAir"] = MIN_HUMI_AIR_PERCENT;
  root["maxHumiGround"] = MAX_HUMI_GROUND_PERCENT;
  root["minHumiGround"] = MIN_HUMI_GROUND_PERCENT;

  char buffer[80];
  root.printTo(buffer, sizeof(buffer));
  String a(buffer);
  Serial.println(a);
  jsonString6 = a;
}

void parseJsonSeven() {
  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["waterTimeSeconds"] = 1;

  char buffer[50];
  root.printTo(buffer, sizeof(buffer));
  String a(buffer);
  Serial.println(a);
  jsonString7 = a;
}

// the loop function runs over and over again forever
void networkLoop() {

  // Check that we are connected.
  if (status != WL_CONNECTED) {
    return;
  }
  // Listen for incoming client requests.
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  //Serial.println("Client connected - start");

  String request = readRequest(&client);
  //Serial.println("Client connected - middle");
  executeRequest(&client, &request);
  client.flush();
  client.stop();
  //Serial.println("Client connected - finsih");
}

// Read the request line. The string from the client ends with a newline.
String readRequest(WiFiClient* client) {

  int counter = 0;
  String request = "";
  // Loop while the client is connected.

  while (client->connected()) {

    // Read available bytes.
    if (client->available()) {
      // Read a byte.
      char c = client->read();
      // Print the value (for debugging).
      //Serial.write(c);
      // Exit loop if end of line.

      if ('\n' == c) {
        //Serial.write("DRINN LEER");
        return request;
      }
      // Add byte to request line.
      request += c;
    }
    //delay(500);
    counter++;
  }

  return request;
}

void executeRequest(WiFiClient* client, String* request) {

  char command = readCommand(request);
  int n = readParam(request);
  if ('O' == command) {

    //char TempString[10];  //  Hold The Convert Data
    //dtostrf(currentTemp,2,2,TempString);
    //sendResponse(client, "Temp: " + String(TempString));

    //pinMode(n, OUTPUT);
  }

  else if ('I' == command) {
    //pinMode(n, INPUT);
  }

  else {
    //Serial.println("DAAAAAAAAAAAAAAAAaa: " + command);
    for(int i = 1; i <= 7; i++) {
      sendResponseJson(client, i);      
    }
    
  }

  //client->stop();
}

// Read the command from the request string.
char readCommand(String* request) {

  String commandString = request->substring(0, 1);
  return commandString.charAt(0);
}

// Read the parameter from the request string.
int readParam(String* request) {

  String value = request->substring(1, 2);
  return value.toInt();
}

void sendResponse(WiFiClient* client, String response) {
  // Send response to client.
  client->println(response);
  // Debug print.
  //Serial.println("sendResponse:");
  //Serial.println(response);
}

void sendResponseJson(WiFiClient* client, int json) {
  switch(json) {
  case 1:
    client->println(jsonString1);
    break;
  case 2:
    client->println(jsonString2);
    break;
  case 3:
    client->println(jsonString3);
    break;
  case 4:
    client->println(jsonString4);
    break;
  case 5:
    client->println(jsonString5);
    break;
  case 6:
    client->println(jsonString6);
    break;
  case 7:
    client->println(jsonString7);
    break;
  }
  // Send response to client.

}

/*
void sendResponseJsonOne(WiFiClient* client) {
  // Send response to client.
  client->println(jsonString1);
}

void sendResponseJsonTwo(WiFiClient* client) {
  // Send response to client.
  client->println(jsonString2);
}

void sendResponseJsonThree(WiFiClient* client) {
  // Send response to client.
  client->println(jsonString3);
}
*/

void printWifiStatus() {

  //Serial.println("WiFi status");
  // Print network name.
  //Serial.print(" SSID: ");
  //Serial.println(WiFi.SSID());
  // Print WiFi shield IP address.
  IPAddress ip = WiFi.localIP();
  //Serial.print(" IP Address: ");
  Serial.println(ip);
  // Print the signal strength.
  long rssi = WiFi.RSSI();
  //Serial.print(" Signal strength (RSSI):");
  //Serial.print(rssi);
  //Serial.println(" dBm");
  /*
  byte mac[6];
   WiFi.macAddress(mac);
   Serial.print("MAC: ");
   Serial.print(mac[5],HEX);
   Serial.print(":");
   Serial.print(mac[4],HEX);
   Serial.print(":");
   Serial.print(mac[3],HEX);
   Serial.print(":");
   Serial.print(mac[2],HEX);
   Serial.print(":");
   Serial.print(mac[1],HEX);
   Serial.print(":");
   Serial.println(mac[0],HEX);
   */
}

void getTemperature() {
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(tempPin_1);
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100 ; //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  currentTemp = temperatureC;
  // return temperatureC; 
}

void getHumitidyGround() {
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(humitidyPin_1);
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float percent = 100 - (voltage * 100) / 5;
  currentHumiGround = voltage;
  currentHumiGroundPercent = percent;
  //return voltage; 
}

void getTempAndHumiAirFromDht() {
  current_dht_humiAir = dht.readHumidity();
  // Read temperature as Celsius
  current_dht_temp = dht.readTemperature();
}

float getHallActivity() {
  int reading = analogRead(hallPin_1); 
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float percent = 100 - (voltage * 100) / 5;
  currentHall = voltage;
  currentHallPercent = percent;
  return voltage; 
}

boolean setRelay()
{
  if(!relayHigh) {
    //Serial.println("Set HIGH");
    digitalWrite(relayPin_1, HIGH);  
    relayHigh = true;
  }
  else {
    //Serial.println("Set LOW");
    digitalWrite(relayPin_1, LOW);
    relayHigh = false;
  }
  return true;
}

boolean setRelay(boolean state)
{
  if(state && (relayHigh != true)) {
    //Serial.println("Set HIGH");
    digitalWrite(relayPin_1, HIGH);  
    relayHigh = true;
  }
  else if(relayHigh != false) {
    //Serial.println("Set LOW");
    digitalWrite(relayPin_1, LOW);
    relayHigh = false;
  }
  return true;
}


boolean checkNeedWater() {
  while(currentHumiGroundPercent <= MAX_HUMI_GROUND_PERCENT && watering) {
    //Serial.println("BRAUCH WASSER!");
    setRelay(LOW);
    // TODO: delay blockiert Arduino komplett, eigentlich ÃƒÂ¼ber sleep...
    delay(5000);
    setRelay(HIGH);
    delay(2000);
    getHumitidyGround();
  }
}






