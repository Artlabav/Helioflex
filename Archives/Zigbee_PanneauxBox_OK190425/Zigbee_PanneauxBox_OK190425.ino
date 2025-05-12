#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include <Wire.h>
#include <INA226.h>
#include "Zigbee.h"

INA226 ina226(0x40);

ZigbeeAnalog zbVoltage(10);
ZigbeeAnalog zbCurrent(11);
ZigbeeAnalog zbPower(12);

void setup() {
  Serial.begin(115200);
  Wire.begin(6, 7); // I2C pour ESP32-C6

  if (!ina226.begin()) {
    Serial.println("INA226 non détecté !");
    while (1);
  }

  ina226.setMaxCurrentShunt(1.5, 0.05);

  zbVoltage.setManufacturerAndModel("Helioflex", "Tension");
  zbCurrent.setManufacturerAndModel("Helioflex", "Courant");
  zbPower.setManufacturerAndModel("Helioflex", "Puissance");

  Zigbee.addEndpoint(&zbVoltage);
  Zigbee.addEndpoint(&zbCurrent);
  Zigbee.addEndpoint(&zbPower);

  if (!Zigbee.begin()) {
    Serial.println("Zigbee KO");
    ESP.restart();
  }
}

void loop() {
  float V = ina226.getBusVoltage();
  float I = ina226.getCurrent();
  float P = ina226.getPower();

  Serial.printf("[Box] V=%.2f V | I=%.2f A | P=%.2f W\n", V, I, P);

  zbVoltage.setAnalogInput(V);
  zbCurrent.setAnalogInput(I);
  zbPower.setAnalogInput(P);

  delay(3000);
}
