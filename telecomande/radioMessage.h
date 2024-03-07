#pragma once
#ifndef RADIOMESSAGE_h
#define RADIOMESSAGE_h

typedef enum 
{
    PA_MIN = 1,
    PA_LOW = 2,
    PA_MAX = 4,
    RESET  = 8
} radioCmd;


typedef struct
{
    char cmd;
    char gauche;
    char droit;
    char check;
} radioMessage;

inline char computeCheck  (radioMessage const & msg) { return msg.cmd ^ msg.gauche ^ msg.droit; }
inline void assignCheck   (radioMessage       & msg) { msg.check = computeCheck(msg); }
inline bool messageIsValid(radioMessage const & msg) { return msg.check == computeCheck(msg); }
#endif

