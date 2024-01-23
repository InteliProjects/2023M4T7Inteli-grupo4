// This file contains the code that will update the location of a user in the "local" variable of Ubidots using the TAG of their card.
// After you create the device and the variable named "local" within Ubidots, you will need to set the values of some variables in this code.
// These variables include: WIFISSID, PASSWORD, TOKEN, DEVICE_ID, TOPIC and LOCAL.
// Do not modify anything else!

// Importing libraries.
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LinkedList.h>

// Configuring the WiFi network.
// You have to change the value of the variables according to the name and password of your WiFi network.
#define WIFISSID "(Enter the name of your WiFi network)"
#define PASSWORD "(Enter the password of your WiFi network)"

// Configuring Ubidots
#define SERVER "things.ubidots.com"
#define PORT 1883
#define VARIABLE_LOCAL "local"

// You have to change the value of the variables according to the token and device ID of your Ubidots account.
#define TOKEN "(Place the token from your Ubidots account)"
#define DEVICE_ID "(Place the device ID from your Ubidots account.)"

// You need to enter the label (name) of the device from your Ubidots account that will be used.
#define TOPIC "/v1.6/devices/(Place the device label from your Ubidots account.)"

//Setting up the prototype PINS.
#define SS_PIN 5
#define RST_PIN 27
#define BUZZER_PIN 25

// This variable is the most important! It is the name of the location, that is, the name of the room where the prototype will be.
// You should change the variable's name to the one you want to appear on Ubidots when swiping the card.
#define LOCAL "(Enter the name of the location where the device will be)"

MFRC522 rfid(SS_PIN, RST_PIN); // Initializing an MFRC522 RFID reader with specified SS_PIN and RST_PIN.

// Initializing a WiFiClient instance for Ubidots and creating a PubSubClient using the WiFiClient.
WiFiClient ubidots;
PubSubClient client(ubidots);

bool cardRead = false; // Variable to track whether a card has been read or not.
String tag = ""; // String variable to store the RFID tag information.


// This function is called once at the beginning of the program.
void setup() {
  Serial.begin(115200); // Initialize serial communication with a baud rate of 115200.
  pinMode(BUZZER_PIN, OUTPUT); // Set the mode of the BUZZER_PIN to OUTPUT.

  SPI.begin(); // Begin SPI communication.
  rfid.PCD_Init(); // Initialize the MFRC522 RFID module.

  // Initialize MQTT and restart the program if initialization fails.
  if (!mqttInit()) {
    delay(3000);
    ESP.restart();
  }
}


// This function is called repeatedly in a loop after setup.
void loop() {
  cardRead = false; // Resetting the cardRead flag to false.

  // Continuously attempting to read the card until it is successfully read.
  while (!cardRead){
    readCartao();
    delay(100);
  }

  // Reconnecting to the MQTT broker if not already connected.
  if (!client.connected()) {
    reconnect();
  }

  // Sending the updated local to Ubidots.
  updateLocal();
}


// Function to read the RFID card.
void readCartao(){
  // Check if a new card is present and read its serial.
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    tag = "";
    tone(BUZZER_PIN, 4500);

    // Concatenating the UID bytes to form the complete RFID tag.
    for (byte i = 0; i < rfid.uid.size; i++) {
      tag += (rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      tag += String(rfid.uid.uidByte[i], HEX);
    }

    delay(1000);
    noTone(BUZZER_PIN);
    cardRead = true; // Setting the cardRead flag to true.
  }
}


// Function to send the updated local to Ubidots.
bool updateLocal() {
  // Creating nested objects for "info" and "context".
  StaticJsonDocument<2000> doc;
  JsonObject info = doc.createNestedObject(VARIABLE_LOCAL);
  JsonObject context = info.createNestedObject("context");
  
  // Setting values.
  info["value"] = 0;
  context["tag"] = tag;
  context["local"] = LOCAL;

  // Serializing JSON document.
  char json[2000];
  serializeJson(doc, json);

  // Publishing the JSON data to the MQTT topic.
  if(!client.publish(TOPIC, json)) {
    return false; // Return true on unsuccessful publication.
  }

  return true; // Return true on successful publication.
}


// Function to initialize MQTT connection.
bool mqttInit() {
  // Attempting to connect to the Wi-Fi network.
  WiFi.begin(WIFISSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Configuring the MQTT client with server details.
  client.setServer(SERVER, PORT);

  // Attempting to connect to the MQTT broker with device credentials.
  while(!client.connect(DEVICE_ID, TOKEN, "")) {
    return false; // Return true on unsuccessful connection.
  }

  return true; // Return true on successful connection.
}


// Function to reconnect to the MQTT broker.
void reconnect() {
  while (!client.connected()) {
    if (client.connect(DEVICE_ID, TOKEN, "")) {} else {delay(400);}
  }
}