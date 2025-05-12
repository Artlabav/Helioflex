#ifndef ZIGBEE_MODE_ZCZR
#error "Zigbee coordinator mode non sélectionné (Tools > Zigbee Mode)"
#endif

#include "Zigbee.h"

ZigbeeAnalog zbVoltage(10);

void setup() {
  Serial.begin(115200);
  Zigbee.addEndpoint(&zbVoltage);

  // Callback quand la tension est mise à jour
  zbVoltage.onAttribute([](uint16_t attrId, ZclValue value) {
    float voltage = value.asFloat();
    Serial.printf("[Coord] Tension reçue : %.2f V\n", voltage);
  });

  if (!Zigbee.begin()) {
    Serial.println("Zigbee Coord KO");
    ESP.restart();
  }

  Serial.println("Zigbee Coord OK – prêt à recevoir.");
}

void loop() {
  delay(5000); // Rien à faire ici, tout passe par les callbacks
}
