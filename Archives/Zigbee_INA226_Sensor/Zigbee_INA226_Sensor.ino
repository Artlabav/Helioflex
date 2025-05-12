#include <Wire.h>
#include <INA226.h>
#include "Zigbee.h"

#define ANALOG_SENSOR_ENDPOINT 10
uint8_t button = BOOT_PIN;

INA226 ina226(0x40);
ZigbeeAnalog zbVoltage = ZigbeeAnalog(ANALOG_SENSOR_ENDPOINT);

void readInaAndSend(void *arg) {
  for (;;) {
    float busVoltage = ina226.getBusVoltage(); // En volts
    int voltageCentiVolts = static_cast<int>(busVoltage * 100.0); // Ex: 3.24V -> 324

    Serial.printf("Tension mesurée: %.2f V (%d centiV)\n", busVoltage, voltageCentiVolts);

    zbVoltage.setAnalogInput(voltageCentiVolts);
    delay(5000); // envoie toutes les 5 sec
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT_PULLUP);

  // --- INA226 INIT ---
  Wire.begin(6, 7); // SDA = GPIO6, SCL = GPIO7
  if (!ina226.begin()) {
    Serial.println("Erreur INA226 !");
    while (1);
  }

  ina226.setAverage(INA226_16_SAMPLES);
  ina226.setBusVoltageConversionTime(INA226_1100_us);
  ina226.setShuntVoltageConversionTime(INA226_1100_us);
  ina226.setModeShuntBusContinuous();

  float maxCurrent = 1.5;
  float shuntResistor = 0.05;
  if (ina226.setMaxCurrentShunt(maxCurrent, shuntResistor) != INA226_ERR_NONE) {
    Serial.println("Erreur de calibration INA226");
    while (1);
  }

  // --- ZIGBEE INIT ---
  zbVoltage.setManufacturerAndModel("HelioFlex", "INA226VoltageSensor");
  zbVoltage.setMinMaxValue(0, 600); // 0V à 6V (en centivolts)
  zbVoltage.setTolerance(1); // 1 centivolt
  Zigbee.addEndpoint(&zbVoltage);

  Serial.println("Starting Zigbee...");
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start, rebooting...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("Zigbee started, connecting to network...");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nZigbee connecté au réseau !");

  // Démarrage de la tâche de lecture
  xTaskCreate(readInaAndSend, "readINA226", 2048, NULL, 10, NULL);
  zbVoltage.setReporting(1, 0, 1);
}

void loop() {
  if (digitalRead(button) == LOW) {
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        Serial.println("Factory Reset Zigbee...");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbVoltage.reportAnalogInput();
  }

  delay(100);
}
