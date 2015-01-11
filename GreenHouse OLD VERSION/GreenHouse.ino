/*
  Greenhouse
  modified 28.10.2014
  by Fabian Binder
 */
#include <SPI.h>
#include <WiFi.h>
#include <SD.h>
#include <mthread.h>

class MainThread : public Thread {

  public:
    MainThread();
  protected:
    bool loop();
};

class NetworkWorkerThread : public Thread {

  public:
    NetworkWorkerThread();
  protected:
    bool loop();
};

class WorkerThread : public Thread {
  public:
   WorkerThread();
   boolean logDataToFile(String data[]);
  protected:
   bool loop();
  private:
   boolean checkNeedWater(); 
  
};

// Contructor
MainThread::MainThread() {
  
}

// Contructor
NetworkWorkerThread::NetworkWorkerThread() {

}

// Contructor
WorkerThread::WorkerThread() {
  
}

//char ssid[] = "WLAN-L8YNC2";     //  your network SSID (name)
//char pass[] = "3910129555865245";    // your network password

//char ssid[] = "HTC Portable Hotspot 2068";     //  your network SSID (name)
//char pass[] = "Faab2014";    // your network password

char ssid[] = "UC-Projects-WiFi";     //  your network SSID (name)
char pass[] = "JF1JJXRL1V3JTB9";    // your network password

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 4;  

const char* sdFilename = "dataLog.txt";

// Modi
boolean watering = false; // set to true for auto watering

//Analog PIN´s
int tempPin_1 = 0; // Analog pin 1 for temperature sensor TMP36 
int humitidyPin_1 = 1; // Analog pin 1 for temperature sensor TMP36 

//Digital PIN´s
int relayPin_1 = 8;
int sdCardPin = 10;

//Relay Status
boolean relayHigh = false;

// Aktuelle Werte Variablen:
float currentTemp = 0;
float currentHumi = 0;
float currentHumiPercent = 0;

// Schwellenwerte:
int MAX_HUMI_PERCENT = 30;

// Server status flag.
int status = WL_IDLE_STATUS;

// Create WiFi server listening on the given port.
WiFiServer server(80);



// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);

  // Wait for serial port to connect. Needed for Leonardo only
  while (!Serial) { ; }

  //initSdCard();

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

  // Relay PIN setzen
  pinMode(relayPin_1, OUTPUT);
  
  // TODO: PIN inversen damit standard LOW ist
  // Da das Relay gleich auf high gesetzt wird beim initalisieren -> ausschalten:
  setRelay();
  main_thread_list->add_thread(new NetworkWorkerThread());
  main_thread_list->add_thread(new MainThread());
  main_thread_list->add_thread(new WorkerThread());

}

bool MainThread::loop() {
  getTemperature();
  getHumitidy();
  this->sleep(2);
  return true;
}

bool NetworkWorkerThread::loop() {
  if(kill_flag)
   return false;
  
  networkLoop();
  this->sleep(1);
  return true; 
}

bool WorkerThread::loop() {
  this->sleep(5);
  //delay(1000);
  String data[5];
  data[0] = "16:35 - 11.12.2014";
  data[1] = "22.5";
  data[2] = "80";
  data[3] = "70";
  data[4] = "100";
  data[5] = "20";
  WorkerThread::logDataToFile(data);
  WorkerThread::checkNeedWater();
  return true;
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
  //client.flush();
  //client.stop();
  Serial.println("Client connected - finsih");
}

// Read the request line. The string from the client ends with a newline.
String readRequest(WiFiClient* client) {
  
  int counter = 0;
  String request = "";
  // Loop while the client is connected.
  
  while (client->connected() && counter <= 1) {
    
    // Read available bytes.
    while (client->available()) {
     
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
     delay(500);
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

float getTemperature() {
  Serial.println(" ------ TEMP -------");
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(tempPin_1);
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  // print out the voltage
  Serial.print(voltage); Serial.println(" volts (Temp)");
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ; //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");
  currentTemp = temperatureC;
  Serial.println(" -------------");
  return temperatureC; 
}

float getHumitidy() {
  Serial.println(" ------ HUMI -------");
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(humitidyPin_1);
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  // print out the voltage
  Serial.print(voltage); Serial.println(" volts (Humi)");
  float percent = 100 - (voltage * 100) / 5;
  Serial.print(percent); Serial.println(" %");
  Serial.println(" -------------");
  currentHumi = voltage;
  currentHumiPercent = percent;
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
  if(state) {
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

boolean WorkerThread::checkNeedWater() {
  while(currentHumiPercent <= MAX_HUMI_PERCENT && watering) {
    Serial.println("BRAUCH WASSER!");
    setRelay(LOW);
    // TODO: delay blockiert Arduino komplett, eigentlich über sleep...
    //delay(2000);
    getHumitidy();
  }
  setRelay(HIGH);
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

boolean WorkerThread::logDataToFile(String data[]) {

  if(sizeof(data) != 6) {
      Serial.println("RAUS! Size: " + sizeof(data));
    return false;
  }
  String resultString = "";
  String startSymbol = "&&";
  String endSymbol = "$$";
  String placeHolder = "++";
  
  String date = "Date " + data[0];
  String temp = "Temp " + data[1];
  String humiGround = "humiGround " + data[2];
  String humiAir = "humiAir " + data[3];
  String light = "light " + data[4];
  String air = "air " + data[5];
  
  resultString += startSymbol;
  //resultString += (date + placeHolder + temp + placeHolder + humiGround + placeHolder + humiAir + placeHolder + light + placeHolder + air);
  resultString += endSymbol;
  
  
  Serial.println(resultString);
  /*File dataFile = SD.open(sdFilename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    //dataFile.print(data);
    dataFile.close();
    return true;
  }
  // if the file isn't open, pop up an error:
  else {
    return false;
  }
  */
  
  return true;
}

String readDataFromFile(String whichData) {

}

