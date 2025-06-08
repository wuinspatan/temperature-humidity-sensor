#include <DHT.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <time.h>

#define DHT_SENSOR_PIN  21 // ESP32 pin GPIO21 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

WiFiServer server(23) ;
WiFiClient client ;
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

const char* ssid = "xx" ;         // Input Your Wi-Fi Name
const char* wifi_password = "xx" ;    // Input Your Wi-Fi Password

void connectToWIFI() {
  delay(1000);
  WiFi.mode(WIFI_STA);

  // Set static IP configuration
  IPAddress local_IP(192, 168, 68, xx); // set your ip address
  IPAddress gateway(192, 168, 68, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);

  WiFi.config(local_IP, gateway, subnet, dns);

  WiFi.begin(ssid, wifi_password); // your actual Wi-Fi password here
  Serial.println("\nConnecting");

  unsigned long startAttempTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WIFI.");
  } else {
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
  }
} 

void connectToServer() {
  server.begin() ;
  server.setNoDelay(true) ;
  Serial.println("Server started...") ;
}//end connectToserver

void setup() {
  Serial.begin(9600) ;
  connectToWIFI() ; // function
  dht_sensor.begin() ; 
  connectToServer() ; //function
}//end setup

unsigned long lastReadTime = 0;
const unsigned long interval = 5000;

void loop() {
  // Check for new Telnet client
  if (server.hasClient()) {
    if (!client || !client.connected()) {
      client = server.available();
      client.println("Connected to ESP32 DHT22 Telnet Monitor");
    } else {
      server.available().stop(); // reject new connections
    }
  }

  // Send sensor data every interval
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= interval) {
    float humi = dht_sensor.readHumidity();
    float tempC = dht_sensor.readTemperature();
    float tempF = dht_sensor.readTemperature(true);

    String output;

    if (isnan(tempC) || isnan(tempF) || isnan(humi)) {
      output = "Failed to read from DHT sensor!\n";
    } else {
      output = "Humidity: " + String(humi, 1) + "%  |  ";
      output += "Temperature: " + String(tempC, 1) + "°C ~ ";
      output += String(tempF, 1) + "°F\n";
    }

    if (client && client.connected()) {
      client.print(output);
    }

    lastReadTime = currentTime;
  }
}//end loop


// Local ESP32 IP: 192.168.68.52 (ESP32 devkit V1)
// pio run -t upload 
// pio device monitor
// click EN botton