/*
  Some Doku 
 */
#include <SPI.h>
#include <Ethernet.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192,168,1,20);

// initialize the library instance:
EthernetClient client;

const unsigned long requestInterval = 60000;  // delay between requests

char serverName[] = "api.twitter.com";  // twitter URL

boolean requested;                   // whether you've made a request since connecting
unsigned long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds

String currentLine = "";            // string to hold the text from server
boolean textFound = false;

void setup() {
  // reserve space for the strings:
  currentLine.reserve(128);

 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // attempt a DHCP connection:
  Serial.println("Attempting to get an IP address using DHCP:");
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Serial.println("failed to get an IP address using DHCP, trying manually");
    Ethernet.begin(mac, ip);
  }
  Serial.print("My address:");
  Serial.println(Ethernet.localIP());
  // connect to Twitter:
  connectToServer();
}



void loop()
{
  if (client.connected()) {
    if (client.available()) {
      
      // read incoming bytes:
      char inChar = client.read();
      Serial.print(inChar);
      currentLine += inChar;
      
      if(currentLine.indexOf("\"text\":\"") > -1) {
        textFound = true; // text found
      } 
      
      if(currentLine.endsWith("\",") && textFound) {
        Serial.println("");
        Serial.println(currentLine.substring(8, currentLine.length() - 2));
        client.stop();     
        currentLine = "";
        textFound = false;
        Serial.println("Will sleep now...");
      } 
      
      if(inChar == ',' && !textFound) {
        currentLine = "";
      }
      
    }   
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(serverName, 80)) {
    Serial.println("making HTTP request...");
    // make HTTP GET request to twitter:
    client.println("GET /search.json?q=%23arduino&rpp=1&include_entities=false&result_type=recent HTTP/1.1");
    client.println("HOST: search.twitter.com");
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}   

