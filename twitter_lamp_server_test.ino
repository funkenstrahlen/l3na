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

void setup() {
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
      // send received data to debug console
      Serial.print(inChar);
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
