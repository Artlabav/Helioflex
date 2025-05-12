/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

// the setup routine runs once when you press reset:
void setup() {
  /*
  * UART1  -> Serial1
  * RX Pin -> GPIO 17
  * TX Pin -> GPIO 16
  * UART Configuration -> SERIAL_8N1
  */
  Serial1.begin(115200,SERIAL_8N1,17,16);
  
}

int i=1;
void loop() {

  Serial1.printf("Hello from UART %d, %x, %c \n", i,i,i);

  i++;

  delay(1000);

}
