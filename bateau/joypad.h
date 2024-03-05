#pragma once
#ifndef JOYPAD_h
#define JOYPAD_h

#include "Arduino.h" 

// Déclaration des boutons et des broches correspondantes
#define pinBoutonA 2
#define pinBoutonB 3
#define pinBoutonC 4
#define pinBoutonD 5
#define pinBoutonE 6
#define pinBoutonF 7
#define pinBoutonK 8

#define x_axis A0
#define y_axis A1

class joypad
{
public:    
    joypad();
    ~joypad();
    
    void calibration(uint8_t const & pin);
    
    void    getAxis(uint8_t &x, uint8_t&y);
    uint8_t getButton();
    bool    getButton(uint8_t button) const;
    
    static inline readButton(uint8_t const &buttons, uint8_t const buttonName) {return buttons & (1<<(buttonName-2));}
    
    inline bool getButtonA(){return readButton(getButton(), pinBoutonA);}
    inline bool getButtonB(){return readButton(getButton(), pinBoutonB);}
    inline bool getButtonC(){return readButton(getButton(), pinBoutonC);}
    inline bool getButtonD(){return readButton(getButton(), pinBoutonD);}
    inline bool getButtonE(){return readButton(getButton(), pinBoutonE);}
    inline bool getButtonF(){return readButton(getButton(), pinBoutonF);}
    inline bool getButtonG(){return readButton(getButton(), pinBoutonK);}
    
    inline uint8_t changed(){return m_changed;}
private:
    uint8_t m_oldPressed;
    uint8_t m_changed;
    
    int16_t m_xMin;
    int16_t m_xMax;
    int16_t m_yMin;
    int16_t m_yMax;        
};



joypad::joypad()
{
    pinMode(pinBoutonA, INPUT); // Déclare la pin/broche du bouton A comme entrée
    pinMode(pinBoutonB, INPUT); // Déclare la pin/broche du bouton B comme entrée
    pinMode(pinBoutonC, INPUT); // Déclare la pin/broche du bouton C comme entrée
    pinMode(pinBoutonD, INPUT); // Déclare la pin/broche du bouton D comme entrée
    pinMode(pinBoutonE, INPUT); // Déclare la pin/broche du bouton E comme entrée
    pinMode(pinBoutonF, INPUT); // Déclare la pin/broche du bouton F comme entrée
    pinMode(pinBoutonK, INPUT); // Déclare la pin/broche du bouton K comme entrée
    
    m_oldPressed = 0;
    m_changed = 0;
    
    m_xMin = ((1 << 10) -1) >> 1;
    m_xMax = (1 << 10) -1;
    m_yMin = m_xMin;
    m_yMax = m_xMax;
}

joypad::~joypad()
{
    
}

void joypad::calibration(uint8_t const & pin)
{
    m_xMin = analogRead(0);
    m_yMin = analogRead(1);
    
    m_xMax = 0;
    m_yMax = 0;
    
    delayMicroseconds(500);
    
    while(digitalRead(pin))
    {
        int x = abs(analogRead(0));
        int y = abs(analogRead(1));
        
        m_xMax = x>m_xMax ? x : m_xMax;
        m_yMax = y>m_yMax ? y : m_yMax;
    }    
}

void joypad::getAxis(uint8_t &x, uint8_t &y)
{
  auto ax = analogRead(0) - m_xMin;
  auto ay = analogRead(1) - m_yMax;
    
	x = map(ax, -m_xMax, +m_xMax, -100, +100);  
	y = map(ay, -m_yMax, +m_yMax, -100, +100);  
}

uint8_t joypad::getButton()
{
	uint8_t buttonMap = ~((PORTD >> 2) & (PORTB << 6));
    
    m_changed = m_oldPressed ^ buttonMap;
    m_oldPressed = buttonMap;
    
	return buttonMap;
}

bool joypad::getButton(uint8_t pin) const
{
	uint8_t bit  = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);

	return static_cast<bool>(*portInputRegister(port) & bit);
}

#endif JOYPAD_h



