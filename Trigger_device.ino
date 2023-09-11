#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
const int pushButtonPin = 2;
// Wi-Fi parameters
String wifiSSID = "";
String wifiPassword = "";
String mqttBroker = "test.mosquitto.org";
WiFiClient client;
int t_id, t_val, s_id, s_val;
int device_id = 1;
PubSubClient mqtt(client);
void connectWiFi();
void connect_mqtt();
void mqttReceiveMessage(char* topic, byte *msg, unsigned int msgLength);

void setup() {
  Serial.begin(115200);
  connectWiFi();
  mqtt.setServer(mqttBroker.c_str(), 1883);
  mqtt.setCallback(mqttReceiveMessage);
}

void mqttReceiveMessage(char* topic, byte*msg, unsigned int msgLength){
  if(String(topic) == "rule"){
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
  t_id = jsonDocument["trigger_id"].as<int>();
  t_val = jsonDocument["trigger_val"].as<int>();
  s_id = jsonDocument["service_id"].as<int>();
  s_val = jsonDocument["service_val"].as<int>();
  if (device_id == t_id){
    Serial.println("Rule Diproses");
  }
  else{
    Serial.println("Tidak dieksekusi");
  }

   }
}

void loop() {
  // Send JSON data to the server
    if(!mqtt.connected()){
    connect_mqtt();
    Serial.println("MQTT Connected");
  }
   if (digitalRead(pushButtonPin) == HIGH) {
    connect_mqtt();
    sendJsonDataToMQTT();
    delay(1000); // Add a delay to avoid multiple sends due to button bouncing
  }
  mqtt.loop();  
}

void sendJsonDataToMQTT() {
  // Create a JSON document
  const size_t capacity = JSON_OBJECT_SIZE(2);  // Adjust the capacity as per your data
  StaticJsonDocument<capacity> doc;

  // Add data to the JSON document
  doc["trigger_id"] = "1";
  doc["trigger_val"] = 0;
  // Serialize the JSON document
  String json;
  serializeJson(doc, json);
  mqtt.publish("trigger", json.c_str());
}

void connect_mqtt(){
  while (!mqtt.connected()){
    Serial.print("Connecting to mqtt...");
    String clientId = "Trigger 1";
    if(mqtt.connect(clientId.c_str()) ){
      mqtt.subscribe("rule");
    }
  }
}

void connectWiFi()
{
  Serial.println("Connecting to wifi...");
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
}
