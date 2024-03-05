#pragma once
#ifndef reboot_h
#define reboot_h

#include <avr/wdt.h>
 
 
void reboot()
{
	Serial.println(F("Reboot asked"));
    wdt_enable(WDTO_15MS); // activer le watchdog
    while (1) {};          // et attendre ...
}

#endif