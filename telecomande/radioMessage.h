#pragma once // Directive pour garantir l'inclusion unique de ce fichier d'en-tête
#ifndef RADIOMESSAGE_h // Directive pour l'inclusion conditionnelle de ce fichier d'en-tête
#define RADIOMESSAGE_h // Directive pour marquer le début de la définition du fichier d'en-tête


// Énumération pour définir les commandes possibles du message radio
typedef enum 
{
    PA_MIN = 1, // Niveau de puissance minimal
    PA_LOW = 2, // Niveau de puissance bas
    PA_MAX = 4, // Niveau de puissance maximal
    RESET  = 8  // Commande de réinitialisation
} radioCmd;

// Structure pour représenter un message radio
typedef struct
{
    char cmd;     // Commande du message
    char gauche;  // Valeur pour le moteur gauche
    char droit;   // Valeur pour le moteur droit
    char check;   // Valeur de contrôle
} radioMessage;

// Fonction pour calculer la valeur de contrôle d'un message radio
inline char computeCheck(radioMessage const & msg) { return msg.cmd ^ msg.gauche ^ msg.droit; }

// Fonction pour assigner la valeur de contrôle à un message radio
inline void assignCheck(radioMessage & msg) { msg.check = computeCheck(msg); }

// Fonction pour vérifier si un message radio est valide
inline bool messageIsValid(radioMessage const & msg) { return msg.check == computeCheck(msg); }

#endif // Fin de la définition du fichier d'en-tête RADIOMESSAGE_h
