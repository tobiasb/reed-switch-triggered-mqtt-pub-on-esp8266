/*
 * Based on the ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Publish Example (https://github.com/tvarnish)
 */
#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

const int ledPin = D6; // This code uses the built-in led for visual feedback that the button has been pressed
const int buttonPin = D5; // Connect your button to pin #D6

// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "<your ssid>";
const char* wifi_password = "<your wifi password>";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "<mqtt host>";
const char* mqtt_topic = "<mqtt topic>";
const char* mqtt_username = "<mqtt broker username>";
const char* mqtt_password = "<mqtt broker password>";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "<publisher name>";

// Initialise the Pushbutton Bouncer object
Bounce bouncer = Bounce();

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Switch the on-board LED off to start with
  digitalWrite(ledPin, HIGH);

  // Setup pushbutton Bouncer object
  bouncer.attach(buttonPin);
  bouncer.interval(5);

  // Begin Serial on 115200
  // Remember to choose the correct Baudrate on the Serial monitor!
  // This is just for debugging purposes
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void publishMessage(char* message) {
  int attempts = 0;
  while(attempts < 3) {
    if (client.publish(mqtt_topic, message)) {
      Serial.println((String)"Message " + message + " sent!");
      return;
    }
    Serial.println("Reconnecting...");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(5);
    attempts++;
  }
}

void loop() {
  bouncer.update();

  if (bouncer.rose()) {
    digitalWrite(ledPin, LOW);
    publishMessage("CLOSED");
  }
  else if (bouncer.fell()) {
    digitalWrite(ledPin, HIGH);
    publishMessage("OPEN");
  }
}
