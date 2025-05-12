#include <Wire.h>
#include <Adafruit_INA219.h>

// Création d'un objet INA219
Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Attendre que le port série soit prêt
  Serial.println("INA219 Test");

  // Initialisation du capteur
  if (!ina219.begin()) {
    Serial.println("Impossible de trouver le capteur INA219. Vérifiez les connexions !");
    while (1) delay(1);
  }
  Serial.println("INA219 initialisé avec succès !");
}

void loop() {
  // Lire la tension
  float busVoltage = ina219.getBusVoltage_V(); // Tension sur le bus (V)
  float shuntVoltage = ina219.getShuntVoltage_mV(); // Tension sur la résistance shunt (mV)
  float current_mA = ina219.getCurrent_mA(); // Courant (mA)
  float power_mW = ina219.getPower_mW(); // Puissance (mW)

  // Affichage des valeurs
  Serial.print("Tension Bus: ");
  Serial.print(busVoltage);
  Serial.println(" V");

  Serial.print("Tension Shunt: ");
  Serial.print(shuntVoltage);
  Serial.println(" mV");

  Serial.print("Courant: ");
  Serial.print(current_mA);
  Serial.println(" mA");

  Serial.print("Puissance: ");
  Serial.print(power_mW);
  Serial.println(" mW");

  Serial.println("----------------------------------");

  delay(2000); // Attendre 2 secondes avant la prochaine lecture
}