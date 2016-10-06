#include <Homie.h>
#include "DHT.h"

#define DHTPIN 14 
#define DHTTYPE DHT21
#define PIN_RELAY 12
#define PIN_LED 13
#define PIN_BUTTON 0
#define INTERVAL 60

unsigned long lastSent = 0;

DHT dht(DHTPIN, DHTTYPE);

HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");
HomieNode switchNode("switch", "switch");

bool switchHandler(HomieRange range, String value) {
  if (value == "true") {
    digitalWrite(PIN_RELAY, HIGH);
    Homie.setNodeProperty(switchNode, "on").send("true");
    Serial.println("Switch is on");
  } else if (value == "false") {
    digitalWrite(PIN_RELAY, LOW);
    Homie.setNodeProperty(switchNode, "on").send("false");
    Serial.println("Switch is off");
  } else {
    return false;
  }

  return true;
}

void setupHandler() {
  Homie.setNodeProperty(temperatureNode, "unit").setRetained(true).send("F");
  Homie.setNodeProperty(humidityNode, "unit").setRetained(true).send("percent");
}

void loopHandler() {
   if (millis() - lastSent >= INTERVAL * 1000UL || lastSent == 0) {
     float humidity = dht.readHumidity();
     float temperature = dht.readTemperature(true);
     Homie.setNodeProperty(temperatureNode, "F").send(String(temperature));
     Homie.setNodeProperty(humidityNode, "F").send(String(humidity));
     lastSent = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  Homie_setFirmware("itead-sonoff", "1.0.0");
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);
  
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);

  switchNode.advertise("on").settable(switchHandler);
  temperatureNode.advertise("unit");
  temperatureNode.advertise("temperature");
  humidityNode.advertise("unit");
  humidityNode.advertise("percent");

  Homie.setup();
}

void loop() {
  Homie.loop();
}
