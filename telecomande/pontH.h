#pragma once
#ifndef PONTH_h
#define PONTH_h

class pontH
{
public:
	inline pontH(int pwmGauchePin, int directionGauchePin, int pwmDroitePin, int directionDroitePin)
  {
    m_pwmPin[0]=pwmGauchePin;
    m_pwmPin[1]=pwmDroitePin;
    m_directionPin[0]=directionGauchePin;
    m_directionPin[1]=directionDroitePin;
  }
	inline  ~pontH(){}

	inline void stopMoteurs();
  inline void vitesseMoteurs(int8_t const &gauche, int8_t const &droit);

private:	
  inline void computeMoteur(int8_t vitesse, int pwmPin, int directionPin);
	
	int m_pwmPin[2];
	int m_directionPin[2];
};



inline void pontH::stopMoteurs()
{
#ifdef BATEAU_DEBUG	
    Serial.print(F("Stop bateau"));
#endif
	digitalWrite(m_pwmPin[0],       LOW);
	digitalWrite(m_pwmPin[1],       LOW);
	digitalWrite(m_directionPin[0], LOW);
	digitalWrite(m_directionPin[1], LOW);
}


inline void pontH::vitesseMoteurs(int8_t const &gauche, int8_t const &droit)
{
	computeMoteur(gauche, m_pwmPin[0], m_directionPin[0]);
	computeMoteur(droit,  m_pwmPin[1], m_directionPin[1]);
}


inline void pontH::computeMoteur(int8_t vitesse, int pwmPin, int directionPin)
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
#endif