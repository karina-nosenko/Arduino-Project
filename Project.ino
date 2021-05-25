/***************************************************
  
  SHENKAR - SMART SYSTEMS
  By: Karina Nosenko
  DATE: May-2021
  
 ****************************************************/
 
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "karinapc"
#define WLAN_PASS       "12345678"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "karina_n"
#define AIO_KEY         "aio_wBhM61QcF9mSjvmUg27PyCAxRsYG"


/**************************** Global State **********************************/


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'shenkar_my_light_level' for publishing.
Adafruit_MQTT_Publish light_level = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/shenkar_my_light_level");

// Setup a feed called 'shenkar_my_light_level' for subscribing to changes.
Adafruit_MQTT_Subscribe photoSensor = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/shenkar_my_light_level");

/*************************** Sketch Code ************************************/

#define LED_BUILTIN 2
int analogPin = 37;     
int led = 15; 
int potValue = 0;

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();



void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Starting..."));
  delay(1000);
  Serial.println(F("\n\n##################################"));
  Serial.println(F("Adafruit MQTT demo - SHENKAR"));
  Serial.println(F("##################################"));
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  Serial.println();

  mqtt.subscribe(&photoSensor);     // Setup MQTT subscription for shenkar_my_light_level feed.

  ledcAttachPin(led, 0);
  ledcSetup(0, 4000, 8);
  pinMode(LED_BUILTIN, OUTPUT);
}



void loop() {
// We must keep this for now
  MQTT_connect();


  // Read the value from the photolight sensor and publish it
  potValue = analogRead(analogPin);
  
  Serial.print(F("\nSending value "));
  Serial.print(potValue);
  Serial.print("...");
  if (! light_level.publish(potValue)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // Read the last value from the server
  Adafruit_MQTT_Subscribe *subscription;
  char* lastValue;
  while ((subscription = mqtt.readSubscription(2000))) {
    if (subscription == &photoSensor) {
      Serial.print(F("Got: "));
      lastValue = (char *)photoSensor.lastread;
      Serial.println(lastValue);
    }
  }
  
  int sensorValue = atoi(lastValue);

  // Update the leds
  ledcWrite(0,sensorValue);

  if(sensorValue>1000)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  
}




// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection");
       mqtt.disconnect();
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
