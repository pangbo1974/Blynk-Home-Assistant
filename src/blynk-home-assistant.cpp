/******************************************************************************
MIT License

Copyright (c) 2017 David Ryan (ptcryan)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

void HandleClients(void);

void callback(char* p_topic, byte* p_payload, unsigned int p_length);
void reconnect(void);
void HandleClients(void);
void toggleLED(void);
void setup(void);
void loop(void);

#define MQTT_VERSION MQTT_VERSION_3_1_1
#define MAX_SRV_CLIENTS 1

// create a timer object
SimpleTimer timer;

int ledState = LOW;

// onboard LED
const int STATUS_LED = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// telnet port for serial monitoring
WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }

  Serial.print("Got a message on ");
  Serial.println(p_topic);
  Serial.print("Payload is ");
  Serial.println(payload.c_str());

  if (String("hass/lamp2/status").equals(p_topic)) {
    if (payload.equals(String("ON"))) {
      Blynk.virtualWrite(V0, HIGH);
    } else {
      Blynk.virtualWrite(V0, LOW);
    }
  }

  if (String("home/right_door/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.setProperty(V1, "offLabel", "DOOR IS OPEN");
      Serial.println("Setting V1 label to DOOR IS OPEN");
    } else {
      Blynk.setProperty(V1, "offLabel", "DOOR IS CLOSED");
      Serial.println("Setting V1 label to DOOR IS CLOSED");
    }
  }

  if (String("home/left_door/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.setProperty(V2, "offLabel", "DOOR IS OPEN");
      Serial.println("Setting V2 label to DOOR IS OPEN");
    } else {
      Blynk.setProperty(V2, "offLabel", "DOOR IS CLOSED");
      Serial.println("Setting V2 label to DOOR IS CLOSED");
    }
  }

  if (String("home/main_floor/kitchen/sink_lights/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.virtualWrite(V3, HIGH);
      Serial.println("Set V3 HIGH");
    } else {
      Blynk.virtualWrite(V3, LOW);
      Serial.println("Set V3 LOW");
    }
  }

  if (String("home/main_floor/den/desk_lamp/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.virtualWrite(V4, HIGH);
      Serial.println("Set V4 HIGH");
    } else {
      Blynk.virtualWrite(V4, LOW);
      Serial.println("Set V4 LOW");
    }
  }

  if (String("home/main_floor/den/desk_lamp/bright/status").equals(p_topic)) {
    if (payload == NULL)  {
      payload = "0";
    }
    Serial.print("Setting desk lamp brightness to ");
    Serial.println(payload);
    Blynk.virtualWrite(V5, payload);
  }

  if (String("home/main_floor/den/overhead_lights/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.virtualWrite(V6, HIGH);
      Serial.println("Set V6 HIGH");
    } else {
      Blynk.virtualWrite(V6, LOW);
      Serial.println("Set V6 LOW");
    }
  }

  if (String("home/main_floor/den/overhead_lights/bright/status").equals(p_topic)) {
    if (payload == NULL)  {
      payload = "0";
    }
    Serial.print("Setting overhead lights brightness to ");
    Serial.println(payload);
    Blynk.virtualWrite(V7, payload);
  }

  if (String("home/main_floor/dining_room/overhead_lights/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.virtualWrite(V8, HIGH);
      Serial.println("Set V8 HIGH");
    } else {
      Blynk.virtualWrite(V8, LOW);
      Serial.println("Set V8 LOW");
    }
  }

  if (String("home/main_floor/dining_room/overhead_lights/bright/status").equals(p_topic)) {
    if (payload == NULL)  {
      payload = "0";
    }
    Serial.print("Setting overhead lights brightness to ");
    Serial.println(payload);
    Blynk.virtualWrite(V9, payload);
  }

  if (String("home/main_floor/living_room/living_room/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.virtualWrite(V10, HIGH);
      Serial.println("Set V10 HIGH");
    } else {
      Blynk.virtualWrite(V10, LOW);
      Serial.println("Set V10 LOW");
    }
  }

  if (String("home/main_floor/living_room/living_room/bright/status").equals(p_topic)) {
    if (payload == NULL)  {
      payload = "0";
    }
    Serial.print("Setting overhead lights brightness to ");
    Serial.println(payload);
    Blynk.virtualWrite(V11, payload);
  }

  if (String("home/main_floor/living_room/living_room_fan_lights/status").equals(p_topic)) {
    if (payload.equals(String("on"))) {
      Blynk.virtualWrite(V12, HIGH);
      Serial.println("Set V12 HIGH");
    } else {
      Blynk.virtualWrite(V12, LOW);
      Serial.println("Set V12 LOW");
    }
  }

  if (String("home/main_floor/living_room/living_room_fan_lights/bright/status").equals(p_topic)) {
    if (payload == NULL)  {
      payload = "0";
    }
    Serial.print("Setting overhead lights brightness to ");
    Serial.println(payload);
    Blynk.virtualWrite(V13, payload);
  }
}

BLYNK_WRITE(V0) {
  int pinData = param.asInt(); // to reset light LED
	if (pinData == 0) {
    client.publish("hass/lamp2/switch", "OFF");
  } else {
    client.publish("hass/lamp2/switch", "ON");
  }
}

BLYNK_WRITE(V1) {
  int pinData = param.asInt();
  if (pinData == 1) {
    client.publish("home/main_floor/garage/right_door/switch/set", "1");
  }
}

BLYNK_WRITE(V2) {
  int pinData = param.asInt();
  if (pinData == 1) {
    client.publish("home/main_floor/garage/left_door/switch/set", "1");
  }
}

BLYNK_WRITE(V3) {
  int pinData = param.asInt();
  if (pinData == 0) {
    client.publish("home/main_floor/kitchen/sink_lights/set", "off");
  } else {
    client.publish("home/main_floor/kitchen/sink_lights/set", "on");
  }
}

BLYNK_WRITE(V4) {
  int pinData = param.asInt();
  String payload;
  if (pinData == 0) {
    payload = "off";
  } else {
    payload = "on";
  }
  client.publish("home/main_floor/den/desk_lamp/set", payload.c_str());
}

BLYNK_WRITE(V5) {
  const char* pinData = param.asStr();
  client.publish("home/main_floor/den/desk_lamp/bright/set", pinData);
}

BLYNK_WRITE(V6) {
  int pinData = param.asInt();
  String payload;
  if (pinData == 0) {
    payload = "off";
  } else {
    payload = "on";
  }
  client.publish("home/main_floor/den/overhead_lights/set", payload.c_str());
}

BLYNK_WRITE(V7) {
  const char* pinData = param.asStr();
  client.publish("home/main_floor/den/overhead_lights/bright/set", pinData);
}

BLYNK_WRITE(V8) {
  int pinData = param.asInt();
  String payload;
  if (pinData == 0) {
    payload = "off";
  } else {
    payload = "on";
  }
  client.publish("home/main_floor/dining_room/overhead_lights/set", payload.c_str());
}

BLYNK_WRITE(V9) {
  const char* pinData = param.asStr();
  client.publish("home/main_floor/dining_room/overhead_lights/bright/set", pinData);
}

BLYNK_WRITE(V10) {
  int pinData = param.asInt();
  String payload;
  if (pinData == 0) {
    payload = "off";
  } else {
    payload = "on";
  }
  client.publish("home/main_floor/living_room/living_room/set", payload.c_str());
}

BLYNK_WRITE(V11) {
  const char* pinData = param.asStr();
  client.publish("home/main_floor/living_room/living_room/bright/set", pinData);
}

BLYNK_WRITE(V12) {
  int pinData = param.asInt();
  String payload;
  if (pinData == 0) {
    payload = "off";
  } else {
    payload = "on";
  }
  client.publish("home/main_floor/living_room/living_room_fan_lights/set", payload.c_str());
}

BLYNK_WRITE(V13) {
  const char* pinData = param.asStr();
  client.publish("home/main_floor/living_room/living_room_fan_lights/bright/set", pinData);
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(_MQTT_CLIENT_ID_, _MQTT_USER_, _MQTT_PASSWORD_)) {
      Serial.println("INFO: connected");

      client.subscribe("home/test");
      client.subscribe("hass/lamp2/status");
      client.loop(); // recommended to loop() by author. https://github.com/knolleary/pubsubclient/issues/98
      client.subscribe("home/right_door/status");
      client.subscribe("home/left_door/status");
      client.loop(); // recommended to loop() by author. https://github.com/knolleary/pubsubclient/issues/98
      client.subscribe("home/main_floor/kitchen/sink_lights/status");
      client.subscribe("home/main_floor/den/desk_lamp/status");
      client.loop(); // recommended to loop() by author. https://github.com/knolleary/pubsubclient/issues/98
      client.subscribe("home/main_floor/den/desk_lamp/bright/status");
      client.subscribe("home/main_floor/den/overhead_lights/status");
      client.loop();
      client.subscribe("home/main_floor/den/overhead_lights/bright/status");
      client.subscribe("home/main_floor/dining_room/overhead_lights/status");
      client.loop();
      client.subscribe("home/main_floor/dining_room/overhead_lights/bright/status");
      client.subscribe("home/main_floor/living_room/living_room/status");
      client.loop();
      client.subscribe("home/main_floor/living_room/living_room/bright/status");
      client.subscribe("home/main_floor/living_room/living_room_fan_lights/status");
      client.loop();
      client.subscribe("home/main_floor/living_room/living_room_fan_lights/bright/status");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Handle any serial IO with connected telent clients
void HandleClients(void) {

  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial1.print("New client: "); Serial1.print(i);
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }

  //check clients for data
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
        while(serverClients[i].available()) Serial.write(serverClients[i].read());
      }
    }
  }

  //check UART for data
  // To echo local debug information (from Serial.println()) place a
  // jumper wire between TX & RX on the ESP8266.
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        delay(1);
      }
    }
  }
}

void toggleLED() {
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
  }
  digitalWrite(STATUS_LED, ledState);
}

void setup() {

  pinMode(STATUS_LED, OUTPUT);
  // toggle status LED
  timer.setInterval(500, toggleLED);


  Serial.begin(115200);
  Serial.println("\nBooting");

  server.begin();
  server.setNoDelay(true);

  Blynk.begin(_BLYNK_AUTH_TOKEN_, _WIFI_SSID_, _WIFI_PASS_, _BLYNK_SERVER_IP_);
  Serial.println("Start Blynk.connect()");

  Serial.println("Connected.");

  ArduinoOTA.setHostname(_MQTT_CLIENT_ID_);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(_MQTT_SERVER_IP_, _MQTT_SERVER_PORT_);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  HandleClients();
  ArduinoOTA.handle();
  client.loop();
  timer.run();
  Blynk.run();
}
