/**
 * @file bateau.cpp
 * @author Votre nom (remplacez par votre nom)
 * @date 2024-03-06
 * @brief Point d'entrée principal du programme qui contrôle un bateau à l'aide d'une télécommande radio.
 *
 * Ce programme utilise une radio nRF24L01 pour recevoir des instructions de la télécommande et 
 * piloter les moteurs du bateau en conséquence. Il utilise également la librairie `pontH.h` pour 
 * contrôler les moteurs et la librairie `reboot.h` pour redémarrer le système si nécessaire.
 */

#include <SPI.h>
#include <RF24.h>

#include "radioMessage.h"
#include "pontH.h"
#include "reboot.h"

// **Définition des broches utilisées**
#define moteurGauchePWM       6
#define moteurDroitPWM        5
#define moteurGaucheDirection 4
#define moteurDroitDirection  3

#define CE_PIN 7
#define CSN_PIN 8

// **Variable pour stocker le timestamp**
unsigned long time = 0;

// **Objet pour la communication radio**
RF24     radio(CE_PIN, CSN_PIN); // instantiate an object for the nRF24L01 transceiver

// **Structure pour contenir le message radio**
radioMessage msg;

// **Objet pour piloter les moteurs**
pontH    pont(moteurGauchePWM, moteurGaucheDirection, moteurDroitPWM, moteurDroitDirection);


// **Tableaux contenant les adresses radio pour l'émetteur et le récepteur**
uint8_t address[][6] = { "BOAT ", "REMOT" };

// **Niveau de puissance de la radio**
uint8_t radioPowerLevel = RF24_PA_MAX;



/**
 * @brief Fonction d'initialisation
 */
void setup()
{
  // Arrêter les moteurs
  pont.stopMoteurs();

  // Configurer la radio
  radio.setPALevel(radioPowerLevel);
  radio.setPayloadSize(sizeof(msg));  // Taille du message radio (défini dans radioMessage.h)
  radio.openWritingPipe(address[1]);  // Configurer le canal d'émission
  radio.openReadingPipe(1, address[0]); // Configurer le canal de réception
  radio.startListening(); // Démarrer l'écoute radio

  #ifdef BATEAU_DEBUG
  Serial.begin(9600); // Initialiser la communication série pour le débogage
  #endif
}

/**
 * @brief Boucle principale du programme
 */
void loop()
{
  // Mettre à jour le timestamp
  time = millis();
  uint8_t pipe;
  if (radio.available(&pipe)) // Vérifier si un message est disponible
  {
    uint8_t bytes = radio.getPayloadSize(); // Obtenir
    radio.read(&msg, bytes);                // Lire le message radio

    if(messageIsValid(msg))// Vérifier la validité du message
    {
      controleBateau(msg.cmd); // Traiter la commande reçue
      pont.vitesseMoteurs(msg.gauche, msg.droit); // Piloter les moteurs en fonction des vitesses reçues
    }
    else
    {
      messageInvalid(); // Signaler la réception d'un message invalide
    }
  }

  // Arrêter les moteurs après 100ms d'inactivité radio
  if(millis() > time+100)
  {
    pont.stopMoteurs();
  }
}

/**
 * @brief Fonction pour contrôler le bateau en fonction de la commande reçue
 * @param cmd La commande reçue de la télécommande
 */
void controleBateau(char &cmd)
{
  // Gérer la commande de redémarrage
  if(cmd & radioCmd::RESET) reboot();

  // Gérer la commande de changement de puissance radio
  if(cmd & 0b111)
  {
    radioPowerLevel = (cmd & radioCmd::PA_MIN) ? RF24_PA_MIN : radioPowerLevel;
    radioPowerLevel = (cmd & radioCmd::PA_LOW) ? RF24_PA_LOW : radioPowerLevel;
    radioPowerLevel = (cmd & radioCmd::PA_MAX) ? RF24_PA_MAX : radioPowerLevel;
    radio.setPALevel(radioPowerLevel);

#ifdef BATEAU_DEBUG
    Serial.print(F("RF24_PA change"));
    Serial.println(radioPowerLevel);
#endif
  }
}

/**
 * @brief Fonction pour signaler un message radio invalide
 */
void messageInvalid()
{
#ifdef BATEAU_DEBUG
  Serial.println("Reception d'un message invalid");
  Serial.print((uint32_t)msg, HEX);
  Serial.println();
#endif
}