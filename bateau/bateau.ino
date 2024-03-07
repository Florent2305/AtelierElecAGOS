/**
 * @file bateau.cpp
 * @author Votre nom (remplacez par votre nom)
 * @date 2024-03-06
 * @brief Point d'entr�e principal du programme qui contr�le un bateau � l'aide d'une t�l�commande radio.
 *
 * Ce programme utilise une radio nRF24L01 pour recevoir des instructions de la t�l�commande et 
 * piloter les moteurs du bateau en cons�quence. Il utilise �galement la librairie `pontH.h` pour 
 * contr�ler les moteurs et la librairie `reboot.h` pour red�marrer le syst�me si n�cessaire.
 */

#include <SPI.h>
#include <RF24.h>

#include "radioMessage.h"
#include "pontH.h"
#include "reboot.h"

// **D�finition des broches utilis�es**
#define moteurGauchePWM       6
#define moteurDroitPWM        5
#define moteurGaucheDirection 4
#define moteurDroitDirection  3

#define CE_PIN 7
#define CSN_PIN 8

// **Variable pour stocker le timestamp**
unsigned long�time = 0;

// **Objet pour la communication radio**
RF24�����radio(CE_PIN, CSN_PIN); // instantiate an object for the nRF24L01 transceiver

// **Structure pour contenir le message radio**
radioMessage msg;

// **Objet pour piloter les moteurs**
pontH����pont(moteurGauchePWM, moteurGaucheDirection, moteurDroitPWM, moteurDroitDirection);


// **Tableaux contenant les adresses radio pour l'�metteur et le r�cepteur**
uint8_t address[][6] = { "BOAT ", "REMOT" };

// **Niveau de puissance de la radio**
uint8_t radioPowerLevel = RF24_PA_MAX;



/**
 * @brief Fonction d'initialisation
 */
void setup()
{
  // Arr�ter les moteurs
  pont.stopMoteurs();

  // Configurer la radio
  radio.setPALevel(radioPowerLevel);
  radio.setPayloadSize(sizeof(msg));��// Taille du message radio (d�fini dans radioMessage.h)
  radio.openWritingPipe(address[1]);��// Configurer le canal d'�mission
  radio.openReadingPipe(1, address[0]); // Configurer le canal de r�ception
  radio.startListening();�// D�marrer l'�coute radio

  #ifdef BATEAU_DEBUG
  Serial.begin(9600); // Initialiser la communication s�rie pour le d�bogage
  #endif
}

/**
 * @brief Boucle principale du programme
 */
void loop()
{
  // Mettre � jour le timestamp
  time = millis();
  uint8_t pipe;
  if (radio.available(&pipe)) // V�rifier si un message est disponible
  {
    uint8_t bytes = radio.getPayloadSize();�// Obtenir
    radio.read(&msg, bytes);���������       // Lire le message radio

    if(messageIsValid(msg))// V�rifier la validit� du message
    {
      controleBateau(msg.cmd); // Traiter la commande re�ue
      pont.vitesseMoteurs(msg.gauche, msg.droit); // Piloter les moteurs en fonction des vitesses re�ues
    }
    else
    {
      messageInvalid(); // Signaler la r�ception d'un message invalide
    }
  }

  // Arr�ter les moteurs apr�s 100ms d'inactivit� radio
  if(millis() > time+100)
  {
    pont.stopMoteurs();
  }
}

/**
 * @brief Fonction pour contr�ler le bateau en fonction de la commande re�ue
 * @param cmd La commande re�ue de la t�l�commande
 */
void controleBateau(char &cmd)
{
  // G�rer la commande de red�marrage
  if(cmd & radioCmd::RESET)�reboot();

  // G�rer la commande de changement de puissance radio
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