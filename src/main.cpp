#include <Arduino.h>
#include <FastLED.h>
#include "timeline.cpp"
#include "effects.cpp"
#include <WiFi.h>
#include <TimeLib.h>

#define debug 0

const char* ssid     = "LTE 5G";
const char* password = "pass2411";

const char* host = "192.168.0.103";

int UTC_offset = 60*60*2;

#define NUM_LEDS 24
#define DATA_PIN 13

CRGB leds[NUM_LEDS];

int timestamp = now();
event sunrise = event(60*60*6.5, 60*60*7);
event pday = event(60*60*7, 60*60*21.5);
event sunset = event(60*60*21.5, 60*60*22);
event night = event(60*60*22,60*60*6.5);
effects eff = effects(NUM_LEDS, leds);


int timestampget(){

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 81;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return 0;
    }

    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return 0;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    String line;
    while(client.available()) {
        line = client.readStringUntil('\r');
        Serial.print(line);
    }
    line = line.substring(0,10);
    Serial.println("closing connection");
    return line.toInt();
}

void timesync(void *pvParameters) {
  for(;;){
      delay(5000);
      int retu = timestampget();
      if (retu == 0) {
          digitalWrite(2, HIGH);
      }
      else{
          digitalWrite(2, LOW);
          setTime(retu);
      }
  }
}

void setup() {
    pinMode(2, OUTPUT);
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    setTime(timestampget());
    xTaskCreatePinnedToCore (timesync,"TASK_1",4096,NULL, 1, NULL, 0);

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    timestamp = (int) millis() / 1000;
    night.timestamp = &timestamp;
    sunrise.timestamp = &timestamp;
    pday.timestamp = &timestamp;
    sunset.timestamp = &timestamp;
}

void loop() {
    eff.tick();

    timestamp = now() + UTC_offset;
    // Serial.println(timestamp);
    if (pday.between()) {
        eff.mode = 5;
        FastLED.setBrightness(255);
        // Serial.println("day");
    }

    if (sunset.between()) {
        eff.mode = 0;
        int blend;
        for (int i = 0; i < NUM_LEDS; i++) {
            blend = map(sunset.betweenMap(), 65535, 0, 0, 255);
            leds[i] = CRGB(map(blend, 0,255, 255, leds[i].r), map(blend, 0,255, 0, leds[i].g), map(blend, 0,255, 0, leds[i].b));
        }
        FastLED.setBrightness(blend);
        // Serial.println("sunset");
    }
    if (night.between()) {
        eff.mode = 4;
        FastLED.setBrightness(0);
        // Serial.println("night");
    }
    if (sunrise.between()) {
        eff.mode = 0;
        int blend;
        for (int i = 0; i < NUM_LEDS; i++) {
            blend = map(sunrise.betweenMap(), 0, 65535, 0, 255);
            leds[i] = CRGB(map(blend, 0,255, 255, leds[i].r), map(blend, 0,255, 0, leds[i].g), map(blend, 0,255, 0, leds[i].b));
        }
        FastLED.setBrightness(blend);
        // Serial.println("sunrise");
    }
    FastLED.show();
}
