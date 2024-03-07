/**
 * @file telecommande_bateau.ino
 * @author Florent LERAY, Jérémy Lefort Besnard
 * @date 2024-03-06
 * @brief Implémente la fonctionnalité de télécommande pour un bateau utilisant un émetteur-récepteur radio nRF24L01 et un joystick.
 *
 * Ce programme contrôle un bateau à distance à l'aide d'un joystick et envoie des messages via un émetteur-récepteur radio nRF24L01.
 * Il inclut des fonctionnalités pour contrôler la direction du bateau (gauche, droite), calibrer le joystick, envoyer une commande de réinitialisation et redémarrer le système.
 */

#define BATEAU_DEBUG

#include <SPI.h>
#include <RF24.h>

#include "joypad.h"       // Inclure la bibliothèque joystick
#include "radioMessage.h" // Inclure la définition de la structure du message radio
#include "reboot.h"       // Inclure la fonction de redémarrage

/**
 * @brief Broche CE (Chip Enable) connectée à l'émetteur-récepteur radio nRF24L01
 */
#define CE_PIN   9

/**
 * @brief Broche CSN (Chip Select) connectée à l'émetteur-récepteur radio nRF24L01
 */
#define CSN_PIN 10

/**
 * @brief Objet émetteur-récepteur radio nRF24L01
 */
RF24 radio(CE_PIN, CSN_PIN);

/**
 * @brief Objet joystick
 */
joypad manette;

/**
 * @brief Structure du message radio
 */
radioMessage msg;

/**
 * @brief Adresses pour l'émetteur et le récepteur
 */
uint8_t address[][6] = { "BOAT ", "REMOT" };

/**
 * @brief Stocke le masque binaire des boutons pressés
 */
uint8_t boutons;

/**
 * @brief Niveau de puissance de transmission radio
 */
uint8_t radioPowerLevel = RF24_PA_MAX;


/**
 * @brief Fonction de configuration
 *
 * Cette fonction initialise la radio, définit son niveau de puissance, sa taille de charge utile et ses adresses,
 * commence à écouter les messages entrants et active éventuellement la communication série pour le débogage.
 */
void setup()
{

#ifdef BATEAU_DEBUG
  Serial.begin(115200);
  while (!Serial) {} // some boards need to wait to ensure access to serial over USB  
#endif
    Serial.println(F("Hello0"));
    radio.begin();
    Serial.println(F("Hello1"));
    radio.setPALevel(radioPowerLevel);
    Serial.println(F("Hello2"));
    radio.setPayloadSize(sizeof(msg));
    Serial.println(F("Hello3"));

    /**
     * @brief Définit l'adresse TX du noeud récepteur dans le tuyau TX
     */
    radio.openWritingPipe(address[1]);
    Serial.println(F("Hello4"));

    /**
     * @brief Définit l'adresse RX du noeud émetteur dans un tuyau RX
     */
    radio.openReadingPipe(1, address[0]); // set the RX address of the TX node into a RX pipe
    Serial.println(F("Hello5"));

  /**
   * @brief Met la radio en mode écoute
   */
  radio.startListening();
  Serial.println(F("Hello6"));
    
  Serial.println(F("Setup finish"));
}

/**
 * @brief Fonction de boucle
 *
 * Cette fonction lit en permanence les entrées du joystick, traite les pressions sur les boutons, prépare et envoie des messages radio,
 * et gère les commandes d'étalonnage, de réinitialisation et de redémarrage.
 */
void loop()
{
    manette.calibration(pinBoutonA);
    Serial.println(F("Loop begins"));
    manette.check();
    Serial.println(F("Manette check finish"));

    /**
     * @brief Lit les valeurs des axes du joystick et les stocke dans la structure du message
     */
    //manette.getAxis((char)msg.gauche, (char)msg.droit);

    /**
     * @brief Lit le masque binaire des boutons pressés
     */
    boutons = manette.getButton();


    /**
     * @brief Traite les événements de pression sur les boutons en fonction de leur position binaire
     */
    if (boutons & 0b00000001)
    {
        msg.gauche = 100;
        msg.droit = 0;
    }
    if (boutons & 0b00000010)
    {
        msg.gauche = 0;
        msg.droit = 100;
    }
    if (boutons & 0b00000100)
    {
        manette.calibration(pinBoutonA);
    }
    if (boutons & 0b00001000)
    {
        msg.gauche = 100;
        msg.droit = 0;
    }
    if (boutons & 0b00010000)
    {
        msg.cmd = radioCmd::RESET;
    }
    if (boutons & 0b00100000)
    {
        reboot();
    }

    /**
     * @brief Evoi le message radio au bateau
     */
    if (!radio.write(&msg, sizeof(msg)))
    {

    }
}
