/*
  Greenhouse
  modified 28.10.2014
  by Fabian Binder
 */
#include <SPI.h>
#include <WiFi.h>
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

MainThread::MainThread() {
  
}

// Contructor
NetworkWorkerThread::NetworkWorkerThread() {

}

//char ssid[] = "WLAN-L8YNC2";     //  your network SSID (name)
//char pass[] = "3910129555865245";    // your network password

//char ssid[] = "HTC Portable Hotspot 2068";     //  your network SSID (name)
//char pass[] = "Faab2014";    // your network password

char ssid[] = "UC-Projects-WiFi";     //  your network SSID (name)
char pass[] = "JF1JJXRL1V3JTB9";    // your network password



int tempPin_1 = 0; // Analog pin 1 for temperature sensor TMP36 

float currentTemp = 0;

// Server status flag.
int status = WL_IDLE_STATUS;

// Create WiFi server listening on the given port.
WiFiServer server(80);



// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);

  // Wait for serial port to connect. Needed for Leonardo only
  while (!Serial) { ; }

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

  main_thread_list->add_thread(new NetworkWorkerThread());
  main_thread_list->add_thread(new MainThread());
}

bool MainThread::loop() {
  getTemperature();
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
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(tempPin_1);
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  // print out the voltage
  Serial.print(voltage); Serial.println(" volts");
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ; //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");
  currentTemp = temperatureC;
  return temperatureC; 
}



