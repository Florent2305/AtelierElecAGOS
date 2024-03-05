#pragma once // Directive pour garantir l'inclusion unique de ce fichier d'en-tête

#ifndef reboot_h // Directive pour l'inclusion conditionnelle de ce fichier d'en-tête
#define reboot_h // Directive pour marquer le début de la définition du fichier d'en-tête

#include <avr/wdt.h> // Inclusion de la bibliothèque AVR pour utiliser les fonctionnalités du watchdog

// Fonction pour redémarrer le microcontrôleur
void reboot()
{
    Serial.println(F("Reboot demandé")); // Affiche un message indiquant que le redémarrage est demandé
    wdt_enable(WDTO_15MS); // Active le watchdog avec un délai de 15 millisecondes
    while (1) {}; // Boucle infinie pour attendre le redémarrage
}

#endif // Fin de la définition du fichier d'en-tête reboot_h
