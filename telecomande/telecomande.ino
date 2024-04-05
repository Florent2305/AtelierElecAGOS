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
#include <math.h>

#include "joypad.h"       // Inclure la bibliothèque joystick
#include "radioMessage.h" // Inclure la définition de la structure du message radio
#include "reboot.h"       // Inclure la fonction de redémarrage

void joystickToMotors(int x, int y, int *left, int *right);

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
uint8_t address[][6] = { "1NODE", "2NODE" };

/**
 * @brief Stocke le masque binaire des boutons pressés
 */
uint8_t boutons;

/**
 * @brief Niveau de puissance de transmission radio
 */
uint8_t radioPowerLevel = RF24_PA_LOW;


/**
 * @brief Fonction de configuration
 *
 * Cette fonction initialise la radio, définit son niveau de puissance, sa taille de charge utile et ses adresses,
 * commence à écouter les messages entrants et active éventuellement la communication série pour le débogage.
 */
void setup()
{
  delay(150);
#ifdef BATEAU_DEBUG
  Serial.begin(115200);
  while (!Serial) {} // some boards need to wait to ensure access to serial over USB  
#endif
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(msg));  // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.stopListening();               // Démarrer l'écoute radio

  manette.lightCalibration();
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
	int8_t x = 0;
	int8_t y = 0;
	
    /**
     * @brief Lit les valeurs des axes du joystick et les stocke dans la structure du message
     */
    manette.getAxis(x, y);

    /**
     * @brief Lit le masque binaire des boutons pressés
     */
    boutons = manette.getButton();

    joystickToMotors(x, y, &msg.gauche, &msg.droit);

    /**
     * @brief Traite les événements de pression sur les boutons en fonction de leur position binaire
     */
    if (boutons & 0b00000001)
    {
        msg.gauche = 100;
        msg.droit = -100;
        Serial.println("Bouton A");
    }
    if (boutons & 0b00000010)
    {
        msg.gauche = -100;
        msg.droit = 100;
        Serial.println("Bouton B");
    }
    if (boutons & 0b00000100)
    {
        manette.calibration(pinBoutonA);
        Serial.println("Bouton C");
    }
    if (boutons & 0b00001000)
    {
        //TODO
        Serial.println("Bouton D");
    }
    if (boutons & 0b00010000)
    {
        Serial.println("Bouton E");
        msg.cmd = radioCmd::RESET;
    }
    if (boutons & 0b00100000)
    {
        Serial.println("Bouton F");
        reboot();
    }

    assignCheck(msg);
    /**
     * @brief Evoi le message radio au bateau
     */
    if (!radio.write(&msg, sizeof(msg)))
    {
      Serial.println(F("msg not send"));
    }
    delay(200);
}


/**
 * @brief Convertit les valeurs X et Y du joystick en valeurs pour les moteurs gauche et droit.
 *
 * Cette fonction utilise des fonctions trigonométriques pour calculer la direction et la magnitude du mouvement du joystick, 
 * puis les convertit en valeurs pour les moteurs gauche et droit.
 *
 * @param x Valeur X du joystick (comprise entre -100 et +100)
 * @param y Valeur Y du joystick (comprise entre -100 et +100)
 * @param left Pointeur vers la variable qui stockera la valeur du moteur gauche
 * @param right Pointeur vers la variable qui stockera la valeur du moteur droit
 */
void joystickToMotors(int x, int y, char *left, char *right)
{
    // Calcul de l'angle du joystick
    float angle = atan2(y, x) * 180 / M_PI;

    // Calcul de la magnitude du mouvement du joystick
    float magnitude = sqrt(pow(x, 2) + pow(y, 2));

    // Conversion de l'angle et de la magnitude en valeurs pour les moteurs
    *left  = (char)(magnitude * cos(angle + 45));
    *right = (char)(magnitude * sin(angle + 45));
}