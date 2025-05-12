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

void setup() {
  Serial.begin(115200); // Initialisation du port série
  Serial.print("ESP32 prêt. Tapez quelque chose :");
}

void loop() {
  if (Serial.available() > 0) {
    // Lire l'entrée utilisateur sous forme de chaîne
    String input = Serial.readStringUntil('\n'); // Lire jusqu'à un saut de ligne
    input.trim(); // Supprimer les espaces et les retours à la ligne

    // Afficher tout ce que l'utilisateur a tapé
    Serial.print("Vous avez tapé : ");
    Serial.print(input);
  }
}
