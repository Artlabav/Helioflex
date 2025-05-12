#include <Wire.h>
#include <Adafruit_INA219.h>

// Création d'une instance du capteur avec l'adresse personnalisée
Adafruit_INA219 ina219(0x42);

void setup() {
  Serial.begin(115200); // Initialisation de la communication série

  
  // Initialisation du capteur INA219
  if (!ina219.begin()) {
    Serial.println("Impossible de trouver le capteur INA219. Vérifiez les connexions et l'adresse I²C !");
    while (1); // Boucle infinie en cas d'erreur
  }

  Serial.println("Capteur INA219 initialisé avec l'adresse 0x42 !");
}

void loop() {
  // Lecture des valeurs
  float busVoltage = ina219.getBusVoltage_V();   // Tension en volts
  float current_mA = ina219.getCurrent_mA();     // Courant en milliampères
  float power_mW = ina219.getPower_mW();         // Puissance en milliwatts

  // Affichage des valeurs sur le moniteur série
  Serial.print("Tension : ");
  Serial.print(busVoltage);
  Serial.println(" V");

  Serial.print("Courant : ");
  Serial.print(current_mA);
  Serial.println(" mA");

  Serial.print("Puissance : ");
  Serial.print(power_mW);
  Serial.println(" mW");

  Serial.println("----------------------------");

  delay(1000); // Attente d'une seconde avant la prochaine lecture
}
