#pragma once // garantie l'inclusion unique de ce fichier
#ifndef PONTH_h // Directive pour l'inclusion conditionnelle de ce fichier d'en-tête
#define PONTH_h // définition du fichier

// Déclaration de la classe pontH
class pontH
{
public:
    // Constructeur prenant en paramètres les broches pour les PWM et les directions des moteurs
    inline pontH(int pwmGauchePin, int directionGauchePin, int pwmDroitePin, int directionDroitePin)
    {
        m_pwmPin[0] = pwmGauchePin; // Initialisation de la broche PWM pour le moteur gauche
        m_pwmPin[1] = pwmDroitePin; // Initialisation de la broche PWM pour le moteur droit
        m_directionPin[0] = directionGauchePin; // Initialisation de la broche de direction pour le moteur gauche
        m_directionPin[1] = directionDroitePin; // Initialisation de la broche de direction pour le moteur droit
    }
    
    // Destructeur de la classe pontH
    inline  ~pontH(){}

    // Méthode pour arrêter les moteurs
    inline void stopMoteurs();
    
    // Méthode pour régler la vitesse des moteurs
    inline void vitesseMoteurs(int8_t const &gauche, int8_t const &droit);

private:
    // Méthode privée pour calculer la commande des moteurs
    inline void computeMoteur(int8_t vitesse, int pwmPin, int directionPin);
    
    // Variables pour stocker les broches des PWM et des directions des moteurs
    int m_pwmPin[2];
    int m_directionPin[2];
};

// Méthode pour arrêter les moteurs
inline void pontH::stopMoteurs()
{
#ifdef BATEAU_DEBUG // Si la macro BATEAU_DEBUG est définie
    Serial.print(F("Arrêt du bateau")); // Affiche un message de débogage
#endif
    // Arrête les deux moteurs en mettant les broches PWM et direction à LOW
    digitalWrite(m_pwmPin[0],       LOW);
    digitalWrite(m_pwmPin[1],       LOW);
    digitalWrite(m_directionPin[0], LOW);
    digitalWrite(m_directionPin[1], LOW);
}

// Méthode pour régler la vitesse des moteurs
inline void pontH::vitesseMoteurs(int8_t const &gauche, int8_t const &droit)
{
    // Calcule la commande des moteurs en fonction des vitesses gauche et droite
    computeMoteur(gauche, m_pwmPin[0], m_directionPin[0]);
    computeMoteur(droit,  m_pwmPin[1], m_directionPin[1]);
}

// Méthode privée pour calculer la commande des moteurs
inline void pontH::computeMoteur(int8_t vitesse, int pwmPin, int directionPin)
{
    // Limite la vitesse entre -100 et 100
    if(vitesse >  100) vitesse =  100;
    if(vitesse < -100) vitesse = -100;

    // Détermine la direction en fonction du signe de la vitesse
    bool direction = vitesse >= 0; 
    int8_t vitesseAbs = abs(vitesse);

    // Inverse la vitesse si la direction est négative
    if(!direction)
    {
        vitesseAbs = 100 - vitesseAbs;
    }
    
    // Convertit la vitesse en valeur PWM
    long pwm = map(vitesseAbs, 0, 100, 0, 255);
    
    // Configure la direction du moteur et le PWM en conséquence
    digitalWrite(directionPin, !direction);
    analogWrite(pwmPin, pwm);
}

#endif // Fin de la définition conditionnalle (si non déjà inclu) du fichier PONTH_h
