#include <Homie.h>

// DHT sensor consts/vars
  #include "DHT.h"
  #define DHTPIN     D3     // What pin we're connected to
  #define DHTTYPE    DHT22  // Can be DHT11, DHT22 (AM2302), DHT21 (AM2301)
  DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

const int MEASURE_INTERVAL = 10; // How often to poll DHT22 for temperature and humidity

unsigned long lastMeasureSent = 0;

HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");

void setupHandler() {
  // Nodes part
  temperatureNode.setProperty("unit").send("c");
  humidityNode.setProperty("unit").send("%");

  // Hardware part
  pinMode(DHTPIN, OUTPUT);
  dht.begin();
  Homie.getLogger() << "DHT " << DHTTYPE << " on pin " << DHTPIN << endl;
}

void loopHandler() {
  if (millis() - lastMeasureSent >= MEASURE_INTERVAL * 1000UL || lastMeasureSent == 0) {
    float temperature = dht.readTemperature(); // Read temperature as Celsius
    float humidity = dht.readHumidity(); // Read humidity as relative [0-100]%
    if (isnan(temperature) || isnan(humidity)) {
      Homie.getLogger() << F("Failed to read from DHT sensor!");
    } else {
      Homie.getLogger() << F("Temperature: ") << temperature << " °C" << endl;
      Homie.getLogger() << F("Humidity   : ") << humidity << " %" << endl;
      temperatureNode.setProperty("degrees").send(String(temperature));
      humidityNode.setProperty("relative").send(String(humidity));
    }
    lastMeasureSent = millis();
  }
}

void setup() {
  Serial.begin(115200); // Required to enable serial output

  Homie_setFirmware("D1Mini-DHT22", "0.17.1.23");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  temperatureNode.advertise("unit");
  temperatureNode.advertise("degrees");

  humidityNode.advertise("unit");
  humidityNode.advertise("relative");

  Homie.setup();
}

void loop() {
  Homie.loop();
}
