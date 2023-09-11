#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

String wifiSSID = "";
String wifiPassword = "";
String mqttBroker = "test.mosquitto.org";
int s_id;
int s_val;
int id = 4;
WiFiClient client;
PubSubClient mqtt(client);

void connectWiFi();
void connect_mqtt();
void mqttReceiveMessage(char* topic, byte *msg, unsigned int msgLength);

void setup()
{
  Serial.begin(115200);
  connectWiFi();
  mqtt.setServer(mqttBroker.c_str(), 1883);
  mqtt.setCallback(mqttReceiveMessage);
}

void loop() {
  if(!mqtt.connected()){
    connect_mqtt();
    Serial.println("MQTT Connected");
    // mqtt.publish("service", " ");
  }
  mqtt.loop();
}


void mqttReceiveMessage(char* topic, byte*msg, unsigned int msgLength){
  if(String(topic) == "service"){
    String pesan;
    for (int i = 0; i < msgLength; i++){
      pesan += (char)msg[i];
    }
    Serial.println(pesan);
    DynamicJsonDocument jsonDocument(200);
    DeserializationError error = deserializeJson(jsonDocument, pesan);
    if (error) {
    Serial.print("Deserialization error: ");
    Serial.println(error.c_str());
    return;
  }

  s_id = jsonDocument["service_id"].as<int>();
  s_val = jsonDocument["service_val"].as<int>();
  // Serial.println(s_id);
  // Serial.println(s_val);
  if (id == s_id){
    Serial.println("Trigger diterima oleh service 1");
  }
  else{
    Serial.println("Tidak dieksekusi");
  }

   }
}
void connect_mqtt(){
  while (!mqtt.connected()){
    Serial.print("Connecting to mqtt...");
    String clientId = "Service 1";
    if(mqtt.connect(clientId.c_str()) ){
      mqtt.subscribe("service");
    }
  }
}

void connectWiFi()
{
  Serial.println("Connecting to wifi...");
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
}
