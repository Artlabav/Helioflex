#include <iostream>

#include <cstring>

extern "C" {

#include "esp_zigbee_api.h"

}
 
class ZigbeeCoordinator {

public:

    void init() {

        esp_zb_init("Zigbee_Coordinator"); // Initialisation du coordinateur

        esp_zb_set_role(ZB_COORDINATOR);  // Définir le rôle comme coordinateur

        esp_zb_start();                   // Démarrer Zigbee

        std::cout << "Zigbee Coordinator started." << std::endl;

    }
 
    void sendMessage(uint16_t addr, uint8_t endpoint, const char* message) {

        esp_zb_send_data(addr, endpoint, (uint8_t*)message, strlen(message));

        std::cout << "Message sent: " << message << std::endl;

    }
 
    void loop() {

        while (true) {

            esp_zb_loop_iteration();

        }

    }

};
 
extern "C" void app_main() {

    ZigbeeCoordinator coordinator;

    coordinator.init();
 
    // Pause pour stabiliser le réseau

    vTaskDelay(5000 / portTICK_PERIOD_MS);
 
    // Exemple : Envoi du message "Coucou" à l'adresse 0x1234, endpoint 1

    coordinator.sendMessage(0x1234, 1, "Coucou");
 
    coordinator.loop();

}

 