#pragma once // garantie l'inclusion unique de ce fichier 
#ifndef JOYPAD_h // inclusion conditionnelle de ce fichier 
#define JOYPAD_h //  inclue le fichier si non-déjà inclu

#include "Arduino.h" // fonctionnalités de la plateforme Arduino

// Déclaration des broches correspondant à chaque bouton du joystick
#define pinBoutonA 2
#define pinBoutonB 3
#define pinBoutonC 4
#define pinBoutonD 5
#define pinBoutonE 6
#define pinBoutonF 7
#define pinBoutonK 8

#define x_axis A0 // Définition de la broche analogique correspondant à l'axe X du joystick
#define y_axis A1 // Définition de la broche analogique correspondant à l'axe Y du joystick

// Déclaration de la classe joypad
class joypad
{
public:
    joypad(); // Déclaration du constructeur
    ~joypad(); // Déclaration du destructeur

    // Méthodes pour calibrer le joystick, récupérer les valeurs des axes et des boutons
    void calibration(uint8_t const & pin);
    void getAxis(uint8_t &x, uint8_t &y);
    uint8_t getButton();
    bool getButton(uint8_t button) const;

    // Méthode statique pour lire l'état d'un bouton spécifique
    static inline readButton(uint8_t const &buttons, uint8_t const buttonName) {return buttons & (1<<(buttonName-2));}

    // Méthodes en ligne pour vérifier l'état de chaque bouton individuellement
    inline bool getButtonA(){return readButton(getButton(), pinBoutonA);}
    inline bool getButtonB(){return readButton(getButton(), pinBoutonB);}
    inline bool getButtonC(){return readButton(getButton(), pinBoutonC);}
    inline bool getButtonD(){return readButton(getButton(), pinBoutonD);}
    inline bool getButtonE(){return readButton(getButton(), pinBoutonE);}
    inline bool getButtonF(){return readButton(getButton(), pinBoutonF);}
    inline bool getButtonG(){return readButton(getButton(), pinBoutonK);}

    inline uint8_t changed(){return m_changed;} // Méthode en ligne pour obtenir l'état de changement des boutons

private:
    uint8_t m_oldPressed; // Variable pour stocker l'état précédent des boutons
    uint8_t m_changed; // Variable pour stocker les changements de l'état des boutons

    int16_t m_xMin; // Valeur minimale pour l'axe X
    int16_t m_xMax; // Valeur maximale pour l'axe X
    int16_t m_yMin; // Valeur minimale pour l'axe Y
    int16_t m_yMax; // Valeur maximale pour l'axe Y
};

// Implémentation du constructeur de la classe joypad
joypad::joypad()
{
    // Configuration des broches des boutons comme entrées
    pinMode(pinBoutonA, INPUT);
    pinMode(pinBoutonB, INPUT);
    pinMode(pinBoutonC, INPUT);
    pinMode(pinBoutonD, INPUT);
    pinMode(pinBoutonE, INPUT);
    pinMode(pinBoutonF, INPUT);
    pinMode(pinBoutonK, INPUT);

    m_oldPressed = 0; // Initialisation de l'état précédent des boutons
    m_changed = 0; // Initialisation de l'état de changement des boutons

    // Initialisation des valeurs min et max des axes X et Y
    m_xMin = ((1 << 10) -1) >> 1;
    m_xMax = (1 << 10) -1;
    m_yMin = m_xMin;
    m_yMax = m_xMax;
}

// Implémentation du destructeur de la classe joypad
joypad::~joypad()
{
    // Rien à faire ici dans notre cas
}

// Méthode pour calibrer le joystick
void joypad::calibration(uint8_t const & pin)
{
    m_xMin = analogRead(0); // Lecture de la valeur minimale de l'axe X
    m_yMin = analogRead(1); // Lecture de la valeur minimale de l'axe Y

    m_xMax = 0; // Initialisation de la valeur maximale de l'axe X
    m_yMax = 0; // Initialisation de la valeur maximale de l'axe Y

    delayMicroseconds(500); // Attente de 500 microsecondes

    // Boucle pour trouver les valeurs maximales des axes X et Y
    while(digitalRead(pin))
    {
        int x = abs(analogRead(0)); // Lecture de la valeur de l'axe X
        int y = abs(analogRead(1)); // Lecture de la valeur de l'axe Y

        // Mise à jour des valeurs maximales des axes X et Y
        m_xMax = x>m_xMax ? x : m_xMax;
        m_yMax = y>m_yMax ? y : m_yMax;
    }
}

// Méthode pour récupérer les valeurs des axes X et Y
void joypad::getAxis(uint8_t &x, uint8_t &y)
{
    auto ax = analogRead(0) - m_xMin; // Lecture de la valeur de l'axe X
    auto ay = analogRead(1) - m_yMax; // Lecture de la valeur de l'axe Y

    // Mappage des valeurs des axes X et Y dans une plage de -100 à 100
    x = map(ax, -m_xMax, +m_xMax, -100, +100);  
    y = map(ay, -m_yMax, +m_yMax, -100, +100);  
}

// Méthode pour récupérer l'état de tous les boutons
uint8_t joypad::getButton()
{
    uint8_t buttonMap = ~((PORTD >> 2) & (PORTB << 6)); // Lecture de l'état de tous les boutons

    // Calcul des changements dans l'état des boutons
    m_changed = m_oldPressed ^ buttonMap;
    m_oldPressed = buttonMap; // Mise à jour de l'état précédent des boutons

    return buttonMap; // Retourne l'état de tous les boutons
}

// Méthode pour récupérer l'état d'un bouton spécifique
bool joypad::getButton(uint8_t pin) const
{
    uint8_t bit  = digitalPinToBitMask(pin); // Extraction du bit correspond

ant à la broche du bouton
    uint8_t port = digitalPinToPort(pin); // Extraction du port correspondant à la broche du bouton

    return static_cast<bool>(*portInputRegister(port) & bit); // Retourne l'état du bouton
}

#endif // Fin de la définition du fichier JOYPAD_h à inclure si non déjà inclu
