/*
  Greenhouse
 modified 28.10.2014
 by Fabian Binder
 */
#include <SPI.h>
#include <WiFi.h>
#include <SD.h>
#include <DHT.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302)

//char ssid[] = "WLAN-L8YNC2";     //  your network SSID (name)
//char pass[] = "3910129555865245";    // your network password

//char ssid[] = "HTC Portable Hotspot 2068";     //  your network SSID (name)
//char pass[] = "Faab2014";    // your network password

boolean wiFiActivated = true;
boolean sdCardActivated = false;

char ssid[] = "UC-Projects-WiFi";     //  your network SSID (name)
char pass[] = "JF1JJXRL1V3JTB9";    // your network password

const int chipSelect = 4;  

// SD Card File
const char* sdFilename = "dataLog.txt";

// Modi
boolean watering = true; // set to true for auto watering

//Analog PINs
int tempPin_1 = 0; // Analog pin 0 for temperature sensor TMP36 
int humitidyPin_1 = 1; // Analog pin 1 for humitidy sensor
int hallPin_1 = 2;

//Digital PINs
int dhtTempHumiPin = 2;
int relayPin_1 = 8;
int sdCardPin = 10;
int pwmAirPin = 6;

//Relay Status
boolean relayHigh = false;

// Aktuelle Werte Variablen:
float currentTemp = 0;
float currentHumiGround = 0;
float currentHumiGroundPercent = 0;
float currentHall = 0;
float currentHallPercent = 0;
float current_dht_humiAir = 0;
float current_dht_temp = 0;

//Log Counter
int maxLogCounter = 10;
int logCounter = 0;

// Schwellenwerte:
int MAX_HUMI_GROUND_PERCENT = 30;

// Server status flag.
int status = WL_IDLE_STATUS;

// Create WiFi server listening on the given port.
WiFiServer server(80);

DHT dht(dhtTempHumiPin, DHTTYPE);

// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);

  // Wait for serial port to connect. Needed for Leonardo only
  while (!Serial) { 
    ; 
  }

  if(sdCardActivated) initSdCard();

  if(wiFiActivated) {
    if (WiFi.status() == WL_NO_SHIELD) {
      // If no shield, print message and exit setup.
      Serial.println("WiFi shield not present");
      status = WL_NO_SHIELD;
      return;
    }

    String version = WiFi.firmwareVersion();
    if (version != "1.1.0") {
      Serial.println("Please upgrade the firmware");
    }

    // Connect to Wifi network.
    while (status != WL_CONNECTED){

      Serial.print("Connecting to Network named: ");
      Serial.println(ssid);
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
  Serial.println("-----------------");
  Serial.println("Temp: " + String(currentTemp) + " C");
  Serial.println("DHT Temp: " + String(current_dht_temp) + " C");
  Serial.println("DHT Humi Air: " + String(current_dht_humiAir) + "%");
  Serial.println("Humi Ground: " + String(currentHumiGroundPercent) + "%");
  Serial.println("Hall: " + String(currentHallPercent) + "%");

  Serial.println("-----------------");
  
  if(wiFiActivated) networkLoop();

  checkNeedWater();
  
  if(sdCardActivated) {
    if(logCounter >= maxLogCounter) {
      /*Serial.println("Write Data data now...");
       String date = String("16:35 - 11.12.2014");
       String temp = String("22.5");
       String humiAir = String("80");
       String humiGround = String("70");
       String light = String("100");
       String air = String("20");
       logDataToFile(date, temp, humiAir, humiGround, light, air);
       logCounter = 0;*/

    }
    else {
      logCounter++;
      readDataFromFile();
    }
  }
  delay(1000);

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
  Serial.println("Client connected - start");

  String request = readRequest(&client);
  Serial.println("Client connected - middle");
  executeRequest(&client, &request);
  client.flush();
  client.stop();
  Serial.println("Client connected - finsih");
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
      Serial.write(c);
      // Exit loop if end of line.
      if ('\n' == c) {
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

    char TempString[10];  //  Hold The Convert Data
    dtostrf(currentTemp,2,2,TempString);
    sendResponse(client, "Temp: " + String(TempString));

    //pinMode(n, OUTPUT);
  }

  else if ('I' == command) {
    //pinMode(n, INPUT);
  }

  else if ('L' == command) {
    //digitalWrite(n, LOW);
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
  Serial.println("sendResponse:");
  Serial.println(response);
}

void printWifiStatus() {

  Serial.println("WiFi status");
  // Print network name.
  Serial.print(" SSID: ");
  Serial.println(WiFi.SSID());
  // Print WiFi shield IP address.
  IPAddress ip = WiFi.localIP();
  Serial.print(" IP Address: ");
  Serial.println(ip);
  // Print the signal strength.
  long rssi = WiFi.RSSI();
  Serial.print(" Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
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
    Serial.println("Set HIGH");
    digitalWrite(relayPin_1, HIGH);  
    relayHigh = true;
  }
  else {
    Serial.println("Set LOW");
    digitalWrite(relayPin_1, LOW);
    relayHigh = false;
  }
  return true;
}

boolean setRelay(boolean state)
{
  if(state && (relayHigh != true)) {
    Serial.println("Set HIGH");
    digitalWrite(relayPin_1, HIGH);  
    relayHigh = true;
  }
  else if(relayHigh != false) {
    Serial.println("Set LOW");
    digitalWrite(relayPin_1, LOW);
    relayHigh = false;
  }
  return true;
}


boolean checkNeedWater() {
  while(currentHumiGroundPercent <= MAX_HUMI_GROUND_PERCENT && watering) {
    Serial.println("BRAUCH WASSER!");
    setRelay(LOW);
    // TODO: delay blockiert Arduino komplett, eigentlich ÃƒÂ¼ber sleep...
    delay(5000);
    setRelay(HIGH);
    delay(2000);
    getHumitidyGround();
  }
}

void initSdCard() {
  Serial.println("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(sdCardPin, OUTPUT);     // change this to 53 on a mega
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

boolean logDataToFile(String _date, String _temp, String _humiAir, String _humiGround, String _light, String _air) {
  /*String resultStringOne = String("");
   
   resultStringOne = String(_date + ' ' + _temp& + ' ' + _humiAir& + ' ' + _humiGround& + ' ' + _light& + ' ' + _air&);
   
   Serial.println(resultStringOne);
   
   if(sdCardActivated) {
   File dataFile = SD.open(sdFilename, FILE_WRITE);
   
   // if the file is available, write to it:
   if (dataFile) {
   Serial.println("Data written to sd!");
   dataFile.println(resultStringOne);
   dataFile.close();
   Serial.println("Data written to sd!");
   return true;
   }
   // if the file isn't open, pop up an error:
   else {
   return false;
   }
   }
   */
  return true;
}

void readDataFromFile() {
  /*
  // re-open the file for reading:
   File myFile = SD.open(sdFilename, FILE_READ);
   if (myFile) {
   Serial.println("read:");
   
   // read from the file until there's nothing else in it:
   while (myFile.available()) {
   Serial.write(myFile.read());
   }
   // close the file:
   myFile.close();
   } 
   else {
   // if the file didn't open, print an error:
   Serial.println("error opening test.txt");
   }
   */
}


