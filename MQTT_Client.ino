#include <ESP8266WiFi.h>
#include <PubSubClient.h> // by Nick ’O Leary (https://pubsubclient.knolleary.net/api.html)

const char* wifi_ssid = "";
const char* wifi_password = "";

const char* mqtt_broker = ""; // MQTT server address
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";

// Define topics to subscribe and/or publish to
const char* topic_sub = "stat/sonoff_01/POWER";
const char* topic_pub = "cmnd/sonoff_01/POWER";
// [...]

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);

void setup() {
  Serial.begin(115200);
  // Initialize and connect to WiFi
  wifi_connect();
  // Initialize MQTT
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  // For receiving messages (subscribing to topic)
  mqtt_client.setCallback(callback);
}

void wifi_connect() {
  delay(10);
  Serial.println("Connecting to WiFi.");
  
  WiFi.mode(WIFI_STA); //Set up as station
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_connect() {
  while (!mqtt_client.connected()) {
    Serial.println("Connecting to MQTT broker.");
    // Connect to MQTT broker
    if (mqtt_client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("MQTT connected");
      // Request subscription to topic(s)
      boolean subscr = mqtt_client.subscribe(topic_sub);
      // [...]
      if (!subscr) {
        Serial.println("Subscription failed.");
      }
    } else {
      Serial.print("MQTT connection failed: ");
      Serial.println(mqtt_client.state());
      Serial.println("Trying again in 5 sec");
      delay(5000);
    }
  }
}

void loop() {
  if (!mqtt_client.connected()) {
    mqtt_connect();
  }
  //MQTT connection management
  mqtt_client.loop();

  publishData();
  delay(5);
}

// Receive message published to a subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  char msg[length];
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  Serial.print("Message: ");
  Serial.println(msg);
}

long lastPub = 0;

void publishData() {
  if (millis() - lastPub > 5000) {
    lastPub = millis();

    char* msg_pub = "ON";
    // Arguments: topic, msg, retain flag
    mqtt_client.publish(topic_pub, msg_pub, true);
    // [...]

    // For numeric message:
    // double state = 21.7;
    // mqtt_client.publish(topic, String(state).c_str(), true);
  }
}
