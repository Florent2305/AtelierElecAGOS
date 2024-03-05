#include <SPI.h> // communication série périphérique
#include <RF24.h> // communication sans fil

#include "radioMessage.h" // contenant la définition de la structure de message radio
#include "pontH.h" // contenant la définition de la classe pontH traitant les deux moteurs
#include "reboot.h" // contenant la définition de la fonction de rebootage du bateau

#define moteurGauchePWM       6 // Définit la broche pour la commande PWM (vitesse) du moteur gauche
#define moteurDroitPWM        5 // commande PWM (vitesse) du moteur droit 
#define moteurGaucheDirection 4 // Définit la broche pour la commande de direction du moteur gauche (avant ou arrière)
#define moteurDroitDirection  3 // direction du moteur droit (avant ou arrière)

#define CE_PIN 7 // Définit la broche d'activation de l'émetteur/récepteur sans fil
#define CSN_PIN 8 // Définit la broche de sélection du canal sans fil

// Déclare une variable pour stocker le temps écoulé depuis le démarrage du programme
// Utilisée pour que le bateau ne continue pas d'avancer indéfiniment
unsigned long  time = 0; 

RF24         radio(CE_PIN, CSN_PIN); // Instancie un objet pour le transcepteur nRF24L01 
radioMessage msg; //stocke les messages radio
pontH        pont(moteurGauchePWM, moteurGaucheDirection, moteurDroitPWM, moteurDroitDirection); // Instancie un objet de la classe pontH pour contrôler les moteurs

uint8_t address[][6] = { "BOAT ", "REMOT" }; // tableau d'adresses pour la communication sans fil
uint8_t radioPowerLevel = RF24_PA_MAX; // stocke le niveau de puissance du module radio

void controleBateau(unsigned char &cmd); // contrôle les moteurs
void messageInvalid(); // gére les messages invalides

void setup()
{
    pont.stopMoteurs(); // Arrête les moteurs du bateau (à l'allumage et au reboot)

    radio.setPALevel(radioPowerLevel); // Configure le niveau de puissance du module radio
    radio.setPayloadSize(sizeof(msg)); // Configure la taille des données à transmettre (doit être de 4 bytes)
    radio.openWritingPipe(address[1]); // Configure l'adresse d'écriture du bateau (adresse TX)
    radio.openReadingPipe(1, address[0]); // Configure l'adresse de lecture de la télécommande (adresse RX)
    radio.startListening(); // Met le module radio en mode écoute

	
#ifdef BATEAU_DEBUG // Si la macro BATEAU_DEBUG est définie (utilisé uniquement pour le debuggage, pas besoin d'en tenir compte pour la compréhension du programme)
    Serial.begin(9600); // Initialise la communication série avec une vitesse de 9600 bauds
#endif
}

void loop()
{
    time = millis(); // Mémorise le temps passé à partir du lancement de la loop (une sorte de chronomètre)

    uint8_t pipe; // stocke le numéro du canal de communication

    if (radio.available(&pipe)) // Vérifie s'il y a un message disponible (donc si bonne réception)
    {
        uint8_t bytes = radio.getPayloadSize(); // Récupère la taille du message
        radio.read(&msg, bytes); // Lit le message

        if (messageIsValid(msg)) // Vérifie si le message est valide
        {
            controleBateau(msg.cmd); // Contrôle le bateau en fonction de la commande reçue
            pont.vitesseMoteurs(msg.gauche, msg.droit); // Régle la vitesse des moteurs
        }
        else
        {
            messageInvalid(); // Gère les messages invalides
        }
    }

    if (millis() > time + 100) // Vérifie si le délai est écoulé
    {
        pont.stopMoteurs(); // Arrête les moteurs du bateau si le délai est écoulé
    }
}

void controleBateau(char &cmd)
{
    if (cmd & radioCmd::RESET)  reboot(); // Redémarre le système si la commande RESET est reçue
    if (cmd & 0b111) // Vérifie si des commandes de changement de puissance sont reçues
    {
        if (cmd & radioCmd::PA_MIN) radioPowerLevel = RF24_PA_MIN; // Régle la puissance à minimale
        if (cmd & radioCmd::PA_LOW) radioPowerLevel = RF24_PA_LOW; // Régle la puissance à basse
        if (cmd & radioCmd::PA_MAX) radioPowerLevel = RF24_PA_MAX; // Régle la puissance à maximale
        radio.setPALevel(radioPowerLevel); // Met à jour le niveau de puissance du module radio

#ifdef BATEAU_DEBUG // Si la macro BATEAU_DEBUG est définie (utilisé uniquement pour le debuggage, pas besoin d'en tenir compte pour la compréhension du programme)
        Serial.print(F("RF24_PA change")); // Affiche un message indiquant un changement de puissance
        Serial.println(radioPowerLevel); // Affiche le niveau de puissance actuel
#endif
    }
}

void messageInvalid()
{
#ifdef BATEAU_DEBUG // Si la macro BATEAU_DEBUG est définie (utilisé uniquement pour le debuggage, pas besoin d'en tenir compte pour la compréhension du programme)
    Serial.println("Reception d'un message invalid"); // Affiche un message indiquant la réception d'un message invalide
    Serial.print((uint32_t)msg, HEX); // Affiche l'adresse mémoire du message
    Serial.println(); // Affiche une ligne vide
#endif
}
