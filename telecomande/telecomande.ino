#include <SPI.h> // Inclusion de la bibliothèque SPI pour la communication SPI
#include <RF24.h> // Inclusion de la bibliothèque RF24 pour la communication sans fil

#include "joypad.h" // Inclusion du fichier d'en-tête pour la classe Joypad
#include "radioMessage.h" // Inclusion du fichier d'en-tête pour la structure RadioMessage
#include "reboot.h" // Inclusion du fichier d'en-tête pour la fonction de redémarrage

#define CE_PIN 9 // Définition de la broche CE
#define CSN_PIN 10 // Définition de la broche CSN

RF24 radio(CE_PIN, CSN_PIN); // Crée un objet pour le module de communication RF24
joypad manette; // Crée un objet pour le joystick (Joypad)
radioMessage msg; // Crée une structure pour les messages radio

// Adresses à utiliser pour la communication
uint8_t address[][6] = { "BOAT ", "REMOT" }; // Adresses de communication
uint8_t boutons; // Variable pour stocker l'état des boutons
uint8_t radioPowerLevel = RF24_PA_MAX; // Niveau de puissance radio

void setup()
{
    radio.setPALevel(radioPowerLevel); // Définit le niveau de puissance radio
    radio.setPayloadSize(sizeof(msg)); // Définit la taille du payload
    radio.openWritingPipe(address[1]); // Définit l'adresse d'écriture
    radio.openReadingPipe(1, address[0]); // Définit l'adresse de lecture
    radio.startListening(); // Met le module RF24 en mode écoute

#ifdef BATEAU_DEBUG
    Serial.begin(9600); // Initialise la communication série pour le débogage
#endif
}

void loop()
{
    unsigned char &g = msg.gauche; // Référence vers la variable gauche du message
    unsigned char &d = msg.droit; // Référence vers la variable droit du message

    manette.getAxis(g, d); // Obtient les valeurs des axes du joystick
    boutons = manette.getButton(); // Obtient l'état des boutons du joystick

    // Gestion des actions en fonction des boutons pressés
    if( boutons & 0b00000001 ) // Bouton A pressé
    {
        msg.gauche = 100;
        msg.droit  = 0;
    }
    if( boutons & 0b00000010 ) // Bouton B pressé
    {
        msg.gauche = 0;
        msg.droit  = 100;
    }
    if( boutons & 0b00000100 ) // Bouton C pressé
    {
        manette.calibration(pinBoutonA); // Calibre le joystick
    }
    if( boutons & 0b00001000 ) // Bouton D pressé
    {
        msg.gauche = 100;
        msg.droit  = 0;
    }
    if( boutons & 0b00010000 ) // Bouton E pressé
    {
        msg.cmd = radioCmd::RESET; // Envoie la commande de réinitialisation
    }
    if( boutons & 0b00100000 ) // Bouton F pressé
    {
        reboot(); // Redémarre le microcontrôleur
    }

    // Envoie le message radio
    if(!radio.write(&msg, sizeof(msg))) // Si l'envoi échoue
    {
        // Gestion de l'échec de l'envoi
    }
}
