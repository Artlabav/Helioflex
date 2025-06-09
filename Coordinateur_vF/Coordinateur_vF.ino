#ifndef ZIGBEE_MODE_ZCZR
#error "Zigbee coordinator mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Zigbee thermostat configuration */
#define THERMOSTAT_ENDPOINT_NUMBER 5
uint8_t button = BOOT_PIN;

ZigbeeThermostat zbThermostat = ZigbeeThermostat(THERMOSTAT_ENDPOINT_NUMBER);

// Save temperature sensor data
float sensor_temp;
float sensor_max_temp ;
float sensor_min_temp;
float sensor_tolerance;

struct tm timeinfo = {};  // Time structure for Time cluster

/****************** Tension/Current sensor handling *******************/
void recieveSensorTemp(float measure) { 
  uint8_t raw = (uint8_t)measure;
  bool isCurrent = raw & 0x80;
  float value = (raw & 0x7F) * 0.5;  // Décodage avec résolution 0.5

  if (isCurrent) {
    Serial.printf("Received current: %.1f A\n", value);
  } else {
    Serial.printf("Received voltage: %.1f V\n", value);
  }
}

void recieveSensorConfig(float min_temp, float max_temp, float tolerance) {
  Serial.printf("Solar sunshade voltage settings: min %.2fV, max %.2fV, tolerance %.2fV\n", min_temp, max_temp, tolerance);
  sensor_min_temp = min_temp;
  sensor_max_temp = max_temp;
  sensor_tolerance = tolerance;
}
/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);

  // Init button switch
  pinMode(button, INPUT_PULLUP);

  // Set callback functions for temperature and configuration receive
  zbThermostat.onTempRecieve(recieveSensorTemp);
  zbThermostat.onConfigRecieve(recieveSensorConfig);

  //Optional: set Zigbee device name and model
  zbThermostat.setManufacturerAndModel("Helioflex", "Coordinator");

  //Optional Time cluster configuration
  //example time January 13, 2025 13:30:30 CET
  timeinfo.tm_year = 2025 - 1900;  // = 2025
  timeinfo.tm_mon = 0;             // January
  timeinfo.tm_mday = 13;           // 13th
  timeinfo.tm_hour = 12;           // 12 hours - 1 hour (CET)
  timeinfo.tm_min = 30;            // 30 minutes
  timeinfo.tm_sec = 30;            // 30 seconds
  timeinfo.tm_isdst = -1;

  // Set time and gmt offset (timezone in seconds -> CET = +3600 seconds)
  zbThermostat.addTimeCluster(timeinfo, 3600);

  //Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbThermostat);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  // When all EPs are registered, start Zigbee with ZIGBEE_COORDINATOR mode
  if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }

  Serial.println("Waiting for solar sunshades to bound to the coordinator");
  while (!zbThermostat.bound()) {
    Serial.printf(".");
    delay(500);
  }

  Serial.println();

  // Get temperature sensor configuration
  zbThermostat.getSensorSettings();
}

void loop() {
  // Handle button switch in loop()
  if (digitalRead(button) == LOW) {  // Push button pressed

    // Key debounce handling
    while (digitalRead(button) == LOW) {
      delay(50);
    }

    // Set reporting interval for temperature sensor
    zbThermostat.setTemperatureReporting(0, 10, 2);
  }
}