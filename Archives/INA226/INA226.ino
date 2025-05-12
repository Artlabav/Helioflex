#include <Wire.h>
#include <INA226.h>

// Adresse I2C par défaut de l'INA226
INA226 ina226(0x40);

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
}

void loop() {
  float busVoltage = ina226.getBusVoltage(); // Tension de bus en volts
  float current = ina226.getCurrent();       // Courant en ampères
  float power = ina226.getPower();           // Puissance en watts

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
  delay(1000);
}
