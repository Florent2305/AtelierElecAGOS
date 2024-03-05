
#include <SPI.h>
#include <RF24.h>

#include "joypad.h"
#include "radioMessage.h"
#include "reboot.h"


#define CE_PIN 9
#define CSN_PIN 10


RF24   radio(CE_PIN, CSN_PIN); // Cree une variable object pour le transceiver nRF24L01 
joypad manette;                // Cree une variable object pour la manette (Joypad)
radioMessage msg;              // Cree une variable structurée pour les messages radio

// Let these addresses be used for the pair
uint8_t address[][6] = { "BOAT ", "REMOT" };
uint8_t boutons;
uint8_t radioPowerLevel = RF24_PA_MAX;


void setup()
{
	radio.setPALevel(radioPowerLevel);
	radio.setPayloadSize(sizeof(msg));    // float datatype occupies 4 bytes    
  radio.openWritingPipe(address[1]);    // set the TX address of the RX node into the TX pipe
  radio.openReadingPipe(1, address[0]); // set the RX address of the TX node into a RX pipe
	radio.startListening();  // put radio in RX mode

#ifdef BATEAU_DEBUG	
	Serial.begin(9600);
#endif	
}

void loop()
{
  unsigned char &g = msg.gauche;
  unsigned char &d = msg.droit;

	manette.getAxis(g, d);
    boutons = manette.getButton();
	
	
	if( boutons & 0b00000001 )
	{
		msg.gauche = 100;
		msg.droit  = 0;
	}
	if( boutons & 0b00000010 )
	{
		msg.gauche = 0;
		msg.droit  = 100;
	}
	if( boutons & 0b00000100 )
	{
		manette.calibration(pinBoutonA);
	}
	if( boutons & 0b00001000 )
	{
		msg.gauche = 100;
		msg.droit  = 0;
	}
	if( boutons & 0b00010000 )
	{
		msg.cmd = radioCmd::RESET;
	}
	if( boutons & 0b00100000 )
	{
		reboot();
	}
	
    if(!radio.write(&msg, sizeof(msg)))
    {

    }
}
