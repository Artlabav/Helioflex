/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

// the setup routine runs once when you press reset:
#include <Arduino.h>

// Définir le numéro de la broche pour la LED
#define LED_PIN 39

void setup() {
  // Configurer le GPIO8 comme sortie
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Éteindre la LED au départ

  // Initialiser la communication série
  Serial.begin(115200);
  Serial.print("ESP32 prêt. Tapez 'ON' pour allumer la LED et 'OFF' pour l'éteindre, 'Blink' pour la faire clignoter");
}

void loop() {
  if (Serial.available() > 0) {
    // Lire l'entrée utilisateur
    String input = Serial.readStringUntil('\n');
    //input.trim(); // Supprimer les espaces et les retours à la ligne

    // Vérifier l'entrée utilisateur
    if (input.equalsIgnoreCase("ON")) {
      digitalWrite(LED_PIN, HIGH); // Allumer la LED
      Serial.print("LED allumée !");
    } else if (input.equalsIgnoreCase("OFF")) {
      digitalWrite(LED_PIN, LOW); // Éteindre la LED
      Serial.print("LED éteinte !");
    } else if (input.equalsIgnoreCase("Blink")) {
      while (true) {
        digitalWrite(LED_PIN, LOW); // Éteindre la LED
        delay(1000);
        digitalWrite(LED_PIN, HIGH); // Éteindre la LED
        delay(1000);
        if (Serial.available() > 0) {
          break;
        }
      }
    } else {
      Serial.print("Commande invalide. Tapez 'ON' ou 'OFF'ou 'Blink'.");
    }
  }
}
