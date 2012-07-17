/*
  Some Doku 
 */
#include <SPI.h>
#include <Ethernet.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192,168,178,120); // that is actually fallback if no dhcp available

// initialize the library instance:
EthernetClient client;

const unsigned long requestInterval = 60000; // delay between requests
char serverName[] = "api.twitter.com"; // twitter api URL

boolean requested; // whether you've made a request since connecting
unsigned long lastAttemptTime = 0; // last time you connected to the server, in milliseconds

String currentLine = ""; // string to hold the text from server
boolean textFound = false; // trigger to check whether the "text" was found in json yet

int ledGreen = 5; // the pin that the LED is attached to
int ledYellow = 6; // the pin that the LED is attached to
int ledRed = 9; // the pin that the LED is attached to

void setup() {

  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);

  analogWrite(ledGreen, 0);
  analogWrite(ledRed, 0); 
  analogWrite(ledYellow, 0); 

  // reserve space for the strings
  currentLine.reserve(200);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect.
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

void loop() {
  if (client.connected()) {
    if (client.available()) {
      
      // read incoming bytes:
      char inChar = client.read();
      //Serial.print(inChar);
      currentLine += inChar;
      
      // check if the currently cached data from server contains the json element "text"
      // we are interested in this element as it contains the tweet we are looking for
      if(currentLine.indexOf("\"text\":\"") > -1) {
        textFound = true; // "text" element was found in json from server
      } 
      
      // check if currently cached data contains the "text" tag AND if we have already received the whole tweet message
      if(currentLine.endsWith("\",") && textFound) {
        // we received the whole tweet \o/
        
        // cut off the "text": at the beginning and the ", at the end
        currentLine = currentLine.substring(8, currentLine.length() - 2);      
                
        Serial.println("");
        Serial.println("##################");  
        Serial.println(currentLine);
        Serial.println("##################");
        setColorFromTweet(currentLine);        
        client.stop(); // we received what we were interested in, so close the connection
        currentLine = ""; // clear data
        textFound = false; // reset triggers
        Serial.print("Will sleep now for "); Serial.print(requestInterval / 1000); Serial.println(" seconds.");
      } 
      
      // if current cached data line does not contain "text" and a "," ist read, delete the cached data
      // this is when a json data field was read, that does not equal the "text" data field
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

/*
 * connect to twitter api server and send a GET request
 */
void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(serverName, 80)) {
    Serial.println("making HTTP request...");
    // make HTTP GET request to twitter:
    // this is the important request to receive the correct data in json format from twitter api
    client.println("GET /search.json?q=%23color&rpp=1&include_entities=false&result_type=recent HTTP/1.1");
    client.println("HOST: search.twitter.com");
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}   

void setColorFromTweet(String currentLine) {

  Serial.println("Trying to extract colors from tweet...");
  
  currentLine.toLowerCase();

  if(currentLine.indexOf("green") > -1) {
    Serial.println("Found green.");
    analogWrite(ledGreen, 255); 
  } else {
    analogWrite(ledGreen, 0); 
  }

  if(currentLine.indexOf("red") > -1) {
    Serial.println("Found red.");
    analogWrite(ledRed, 255); 
  } else {
    analogWrite(ledRed, 0); 
  }

  if(currentLine.indexOf("yellow") > -1) {
    Serial.println("Found yellow.");
    analogWrite(ledYellow, 255); 
  }  else {
    analogWrite(ledYellow, 0); 
  } 
}
