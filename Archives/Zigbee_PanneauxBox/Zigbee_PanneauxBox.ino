#include <Wire.h>
#include <INA226.h>
#include "Zigbee.h"

INA226 ina226(0x40);
ZigbeeAnalog zbVoltage(10);  // Endpoint ID = 10

void setup() {
  Serial.begin(115200);
  Wire.begin(6, 7);

  if (!ina226.begin()) {
    Serial.println("INA226 not found!");
    while (1);
  }

  ina226.setMaxCurrentShunt(1.5, 0.05);

  zbVoltage.setManufacturerAndModel("Helioflex", "Tension");
  Zigbee.addEndpoint(&zbVoltage);

  if (!Zigbee.begin()) {
    Serial.println("Zigbee init failed");
    ESP.restart();
  }
}

void loop() {
  float voltage = ina226.getBusVoltage();
  zbVoltage.setAnalogInput(voltage);

  Serial.printf("[Box] Tension = %.2f V\n", voltage);
  delay(3000);
}
