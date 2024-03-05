
#include <SPI.h>
#include <RF24.h>

#include "radioMessage.h"
#include "pontH.h"
#include "reboot.h"

#define moteurGauchePWM       6
#define moteurDroitPWM        5
#define moteurGaucheDirection 4
#define moteurDroitDirection  3

#define CE_PIN 7
#define CSN_PIN 8

unsigned long  time = 0;

RF24         radio(CE_PIN, CSN_PIN); // instantiate an object for the nRF24L01 transceiver
radioMessage msg;
pontH        pont(moteurGauchePWM, moteurGaucheDirection, moteurDroitPWM, moteurDroitDirection);


uint8_t address[][6] = { "BOAT ", "REMOT" };
uint8_t radioPowerLevel = RF24_PA_MAX;


void controleBateau(unsigned char &cmd);
void messageInvalid();

void setup()
{
    pont.stopMoteurs();
	
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
	time = millis();
	uint8_t pipe;
	if (radio.available(&pipe)) // is there a message payload? get the pipe number that recieved it
	{
        uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
        radio.read(&msg, bytes);                 // fetch msg from FIFO

	
		if(messageIsValid(msg))
		{
			controleBateau(msg.cmd);
			pont.vitesseMoteurs(msg.gauche, msg.droit);
		}
		else
		{
			messageInvalid();
		}
    }
	
	if(millis() > time+100)
	{
		pont.stopMoteurs();
	}
}



void controleBateau(char &cmd)
{
	if(cmd & radioCmd::RESET)  reboot();
	if(cmd & 0b111)
	{
	    if(cmd & radioCmd::PA_MIN) radioPowerLevel=RF24_PA_MIN;
	    if(cmd & radioCmd::PA_LOW) radioPowerLevel=RF24_PA_LOW;
	    if(cmd & radioCmd::PA_MAX) radioPowerLevel=RF24_PA_MAX;
		radio.setPALevel(radioPowerLevel);
#ifdef BATEAU_DEBUG	
        Serial.print(F("RF24_PA change"));
        Serial.println(radioPowerLevel);
#endif
	}
}

void messageInvalid()
{	
#ifdef BATEAU_DEBUG	
	Serial.println("Reception d'un message invalid");
	Serial.print((uint32_t)msg, HEX);
	Serial.println();
#endif
}