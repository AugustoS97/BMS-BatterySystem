#ifndef const_h
#define const_h

#include <stdint.h>

//Parametros de las NTC
const int BETTA = 3977; //Betta de la NTC
const int To = 25+273 ; //To es 25ºC para NTC
const int Ro = 10000 ; //Ro es 10kOhm
const int Raux = 9800; //Raxu del divisor de tension es de 9.8kOhm
const int Vcc = 5; //Vcc es de 5V para sensor de NTC

//Parametro de la celda
const float internal_resistor = 0.066; //66 mOhm

const uint8_t PIN_SYNC_MUX= 8; //Pin del CS del ADG731  (D8 que es el PB0 en ATMega 328P)
const uint8_t TOTAL_IC = 1; //Numero de IC a emplear

#endif
