/**
 * @file pontH.h
 * @author Florent LERAY, J�r�my Lefort Besnard
 * @date 2024-03-06
 * @brief D�finit la classe `pontH` pour contr�ler un pont en H et piloter deux moteurs � courant continu.
 *
 * Cette classe permet de piloter deux moteurs � courant continu en fonction des valeurs de vitesse fournies 
 * pour la direction gauche et droite. Elle utilise des broches PWM et de direction pour contr�ler la vitesse 
 * et le sens de rotation des moteurs.
 */

#pragma once
#ifndef PONTH_h
#define PONTH_h

class pontH
{
public:
  /**
   * @brief Constructeur de la classe pontH
   *
   * Ce constructeur initialise les broches PWM et de direction pour les deux moteurs.
   * @param pwmGauchePin Broche PWM du moteur gauche
   * @param directionGauchePin Broche de direction du moteur gauche
   * @param pwmDroitePin Broche PWM du moteur droit
   * @param directionDroitePin Broche de direction du moteur droit
   */
  inline pontH(int pwmGauchePin, int directionGauchePin, int pwmDroitePin, int directionDroitePin)
  {
    m_pwmPin[0] = pwmGauchePin;
    m_pwmPin[1] = pwmDroitePin;
    m_directionPin[0] = directionGauchePin;
    m_directionPin[1] = directionDroitePin;
  }

  /**
   * @brief Destructeur de la classe pontH (ne fait rien)
   */
  inline ~pontH() {}

  /**
   * @brief Arr�ter les moteurs
   *
   * Cette fonction arr�te les deux moteurs en mettant les broches PWM � LOW et les broches de direction 
   * � LOW.
   */
  inline void stopMoteurs()
  {
#ifdef BATEAU_DEBUG
    Serial.print(F("Arr�t du bateau\n"));
#endif
    digitalWrite(m_pwmPin[0],       LOW);
    digitalWrite(m_pwmPin[1],       LOW);
    digitalWrite(m_directionPin[0], LOW);
    digitalWrite(m_directionPin[1], LOW);
  }

  /**
   * @brief D�finir la vitesse des moteurs
   *
   * Cette fonction d�finit la vitesse des deux moteurs en fonction des valeurs de vitesse fournies 
   * pour la direction gauche et droite. Les valeurs de vitesse doivent �tre comprises entre -100 et 100.
   * @param gauche Vitesse du moteur gauche (-100 pour la vitesse maximale en arri�re, 0 pour � l'arr�t, 100 pour la vitesse maximale en avant)
   * @param droit Vitesse du moteur droit (-100 pour la vitesse maximale en arri�re, 0 pour � l'arr�t, 100 pour la vitesse maximale en avant)
   */
  inline void vitesseMoteurs(int8_t const &gauche, int8_t const &droit)
  {
    computeMoteur(gauche, m_pwmPin[0], m_directionPin[0]);
    computeMoteur(droit,  m_pwmPin[1], m_directionPin[1]);
  }

private:
  /**
   * @brief Calculer la configuration d'un moteur en fonction de sa vitesse
   *
   * Cette fonction interne calcule la configuration d'un moteur en fonction de la valeur de vitesse fournie.
   * @param vitesse Vitesse du moteur (-100 pour la vitesse maximale en arri�re, 0 pour � l'arr�t, 100 pour la vitesse maximale en avant)
   * @param pwmPin Broche PWM du moteur
   * @param directionPin Broche de direction du moteur
   */
  inline void computeMoteur(int8_t vitesse, int pwmPin, int directionPin)
  {
      if(vitesse >  100) vitesse =  100;
      if(vitesse < -100) vitesse = -100;
  
      bool direction = vitesse >= 0;    
      int8_t vitesseAbs = abs(vitesse);
      
      if(!direction)
      {
          vitesseAbs = 100 - vitesseAbs;
      }
      
      long pwm = map(vitesseAbs, 0, 100, 0, 255);
      
      digitalWrite(directionPin, !direction);
      analogWrite(pwmPin, pwm);
  }



private:
  /**
   * @brief Tableau stockant les broches PWM des moteurs
   */
  int m_pwmPin[2];

  /**
   * @brief Tableau stockant les broches de direction des moteurs
   */
  int m_directionPin[2];
};

#endif
