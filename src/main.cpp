#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_sleep.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"                                  // DHT Sensor Lib
#include "user_config.h"                          // Fixed user configurable options
#include "secrets.h"                          // Fixed user configurable options



WiFiServer server(WEB_PORT);

/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

// Initialize DHT sensor.
DHT dht(PIN_DHT, DHTTYPE);

// Client variables
long lastMsg = 0;

void setup() {
  dht.begin();                                    // initialize the DHT sensor

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);               // Connect with Wifi

  while(WiFi.status() != WL_CONNECTED) {          // Try to create connection to WiFi
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Config pins
  pinMode(16, OUTPUT);
  pinMode(PIN_POWER, INPUT);
  digitalWrite(16, LOW);


  if (MQTT == 1) {                                // MQ Start
    client.setServer(MQTT_BROKER, MQTT_PORT);     // Configure the MQTT server
    Serial.println("");
    Serial.print("Set MQTT Broker: ");
    Serial.println(MQTT_BROKER);
    Serial.print("Set MQTT Port: ");
    Serial.println(MQTT_PORT);
  } else {                                        // MQ Stop
    Serial.println("");
    Serial.println("MQTT off");
  }

  if (WEB_SERVER == 1) {                          // Webserver Start
    server.begin();
    Serial.println("Webserver is online");
    Serial.println("Set Webserver Port to: ");
    Serial.println(WEB_PORT);
  } else {                                        // Webserver Stop
    Serial.println("");
    Serial.println("Webserver is offline");
  }
}

// MQTT Connect
void mqttconnect() {
  while (!client.connected()) {                   // Try to connect to MQTT
    Serial.print("MQTT connecting...");
    if (client.connect(CLIENT)) {                 // Connect
      Serial.println("connected");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);                                // Wait 5 seconds before retrying
    }
  }
}




// Read the water level and return a readable value
float getWater() {
  float water = analogRead(PIN_SOIL);
  water = map(water, 0, 4095, 0, 1023);
  water = constrain(water, 0, 1023);
  return water;
}

// Read the light level and return a readable value
float getLight() {
  float light = analogRead(PIN_LIGHT);
  light = map(light, 0, 4095, 0, 1023);
  light = constrain(light, 0, 1023);
  return light;
}

// Read the temperature and return a readable value
float getTemperature() {
  float temperature = dht.readTemperature();
  if (TEMP_UNIT == "F") {
    temperature *= 33,8;
  }
  temperature += OFFSET_TEMPERATURE;
  return temperature;
}

// Read the humidity level and return a readable value
float getHumidity() {
  return (float)dht.readHumidity() + OFFSET_HUMIDITY;
}
// Build JSON
String toJson(){
  char body[1024] = "";
  strcat(body, "{\"water\":\"");
  strcat(body, String((float)getWater()).c_str());
  strcat(body, "\",\"luminosity\":\"");
  strcat(body, String((float)getLight()).c_str());
  strcat(body, "\",\"humidity\":\"");
  strcat(body, String((float)getHumidity()).c_str());
  strcat(body, "\",\"temperature\":\"");
  strcat(body, String((float)getTemperature()).c_str());
  strcat(body, "\"}");
  return body;
}

void loop() {

  // -- Web-Server -------------------------------------
  if (WEB_SERVER == 1) {
    WiFiClient wifiClient = server.available();   // listen for incoming clients
    if (wifiClient) {

      // send a standard http response header
      wifiClient.println("HTTP/1.1 200 OK");
      wifiClient.println("Content-Type: application/json");
      wifiClient.println("Connection: close");
      wifiClient.println();
      wifiClient.println(toJson());
      // give the web browser time to receive the data
      delay(10);

      wifiClient.stop();                          // close the connection
    }
  } else {
    //this does not work
    //esp_deep_sleep_enable_timer_wakeup(MQTT_INTERVAL * 1000000);    // Go to deepsleep if the Webserver is offline
    //esp_deep_sleep_start();
  }
}
