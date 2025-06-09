#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"
#include <Wire.h>
#include <INA226.h>

// Adresse I2C par défaut de l'INA226
INA226 ina226(0x40);

/* Zigbee temperature sensor configuration */
#define TEMP_SENSOR_ENDPOINT_NUMBER 10
uint8_t button = BOOT_PIN;

// Optional Time cluster variables
struct tm timeinfo;
struct tm *localTime;
int32_t timezone;

ZigbeeTempSensor zbTempSensor = ZigbeeTempSensor(TEMP_SENSOR_ENDPOINT_NUMBER);

/************************ Temp sensor *****************************/
uint8_t encodeValue(bool isCurrent, float value) {
  uint8_t data = (uint8_t)(value / 0.5);  // résolution 0.5 → max 127
  if (data > 127) data = 127;
  return (isCurrent << 7) | (data & 0x7F);
}

static void temp_sensor_value_update(void *arg) {
  for (;;) {
    float busVoltage = ina226.getBusVoltage(); // Tension de bus en volts
    uint8_t encodedVoltage = encodeValue(false, busVoltage); // ID 0 → tension
    zbTempSensor.setTemperature((float)encodedVoltage);
    printf("Send voltage: %.2f V (raw=%d)\n", busVoltage, encodedVoltage);
    delay(5000);
    float current = ina226.getCurrent();
    uint8_t encodedCurrent = encodeValue(true, current); // ID 1 → courant
    zbTempSensor.setTemperature((float)encodedCurrent);
    printf("Send current: %.2f A (raw=%d)\n", current, encodedCurrent);
    delay(5000);
  }
}
/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
 Serial.println("Initialisation de l'INA226...");

  // Initialisation du bus I2C
  Wire.begin(6, 7); // SDA = GPIO6, SCL = GPIO7

  // Initialisation de l'INA226
  if (!ina226.begin()) {
    Serial.println("Erreur : Impossible de détecter l'INA226. Vérifiez les connexions !");
    while (1);
  }

  Serial.println("INA226 détecté avec succès !");

  // Configuration de l'INA226
  ina226.setAverage(INA226_16_SAMPLES);                      // Moyennage sur 16 échantillons
  ina226.setBusVoltageConversionTime(INA226_1100_us);        // Temps de conversion tension : 1.1 ms
  ina226.setShuntVoltageConversionTime(INA226_1100_us);      // Temps de conversion shunt : 1.1 ms
  ina226.setModeShuntBusContinuous();                        // Mode continu pour le bus et le shunt

  // Calibrage
  float maxCurrent = 1.5;    // Max courant en A
  float shuntResistor = 0.05; // Résistance shunt en Ω

  int status = ina226.setMaxCurrentShunt(maxCurrent, shuntResistor);
  if (status != INA226_ERR_NONE) {
    Serial.print("Erreur de calibration : ");
    if (status == INA226_ERR_SHUNTVOLTAGE_HIGH) {
      Serial.println("Tension shunt trop élevée (> 80 mV). Réduisez le courant max ou la résistance shunt.");
    } else if (status == INA226_ERR_MAXCURRENT_LOW) {
      Serial.println("Courant max trop faible (< 0.001 A). Augmentez le courant max.");
    } else if (status == INA226_ERR_SHUNT_LOW) {
      Serial.println("Résistance shunt trop faible (< 0.001 Ω). Augmentez la résistance.");
    } else if (status == INA226_ERR_NORMALIZE_FAILED) {
      Serial.println("Normalisation échouée. Vérifiez les paramètres.");
    } else {
      Serial.println("Erreur inconnue.");
    }
    while (1); // Stoppe l'exécution
  }

  Serial.println("Configuration et calibrage terminés !");

  // Init button switch
  pinMode(button, INPUT_PULLUP);

  // Optional: set Zigbee device name and model
  zbTempSensor.setManufacturerAndModel("Helioflex", "ZigbeeTempSensor");

  // Set minimum and maximum temperature measurement value (10-50°C is default range for chip temperature measurement)
  zbTempSensor.setMinMaxValue(0, 255);

  // Optional: Set tolerance for temperature measurement in °C (lowest possible value is 0.01°C)
  zbTempSensor.setTolerance(0.01);

  // Optional: Time cluster configuration (default params, as this device will revieve time from coordinator)
  zbTempSensor.addTimeCluster();

  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbTempSensor);

  Serial.println("Starting Zigbee...");
  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Optional: If time cluster is added, time can be read from the coordinator
  timeinfo = zbTempSensor.getTime();
  timezone = zbTempSensor.getTimezone();

  Serial.println("UTC time:");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  time_t local = mktime(&timeinfo) + timezone;
  localTime = localtime(&local);

  Serial.println("Local time with timezone:");
  Serial.println(localTime, "%A, %B %d %Y %H:%M:%S");

  // Start Temperature sensor reading task
  xTaskCreate(temp_sensor_value_update, "temp_sensor_update", 2048, NULL, 10, NULL);

  // Set reporting interval for temperature measurement in seconds, must be called after Zigbee.begin()
  // min_interval and max_interval in seconds, delta (temp change in 0,1 °C)
  // if min = 1 and max = 0, reporting is sent only when temperature changes by delta
  // if min = 0 and max = 10, reporting is sent every 10 seconds or temperature changes by delta
  // if min = 0, max = 10 and delta = 0, reporting is sent every 10 seconds regardless of temperature change
  zbTempSensor.setReporting(0, 10, 0);
}

void loop() {
  float busVoltage = ina226.getBusVoltage(); // Tension de bus en volts
  float current = ina226.getCurrent();       // Courant en ampères
  float power = ina226.getPower();           // Puissance en watts
  //zbTempSensor.setTemperature(busVoltage);
  // Affichage des mesures
  Serial.print("Tension de bus : ");
  Serial.print(busVoltage);
  Serial.println(" V");

  Serial.print("Courant : ");
  Serial.print(current);
  Serial.println(" A");

  Serial.print("Puissance : ");
  Serial.print(power);
  Serial.println(" W");

  Serial.println("------------------------------");
  delay(10000);

  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbTempSensor.reportTemperature();
  }
  delay(100);
}