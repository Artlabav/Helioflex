#include <iostream>

#include <cstring>

extern "C" {

#include "esp_zigbee_api.h"

}
 
class ZigbeeEndpoint {

public:

    void init() {

        esp_zb_init("Zigbee_Device"); // Initialisation du périphérique Zigbee

        esp_zb_set_role(ZB_DEVICE);  // Définir le rôle comme appareil

        esp_zb_start();              // Démarrer Zigbee

        std::cout << "Zigbee Device started and waiting for messages." << std::endl;

    }
 
    void onMessageReceived(const uint8_t* data, uint16_t length) {

        std::string message(data, data + length);

        std::cout << "Message received: " << message << std::endl;

    }
 
    void loop() {

        while (true) {

            esp_zb_loop_iteration();

        }

    }

};
 
// Callback pour les messages reçus

extern "C" void zb_message_callback(const uint8_t* data, uint16_t length) {

    ZigbeeEndpoint receiver;

    receiver.onMessageReceived(data, length);

}
 
extern "C" void app_main() {

    ZigbeeEndpoint endpoint;

    endpoint.init();
 
    // Enregistrement du callback pour recevoir des messages

    esp_zb_register_message_callback(zb_message_callback);
 
    endpoint.loop();

}

 