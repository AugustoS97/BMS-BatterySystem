#ifndef ADG731_lib_h
#define ADG731_lib_h

#include <Arduino.h>
#include <stdint.h>

//#define SERIAL_DEBUG
/************************************
  Funcion que lee hasta 32 NTC y las almacena en un array pasado por referencia
 **************************************/
float measure_all_temp(float array_temp[], const uint8_t N_NTC, const int BETTA, const int To, const int Ro,const int Raux, const int Vcc, const uint8_t AnalogInput, const uint8_t PIN_SYNC_MUX) ;

/************************************
  Funcion para seleccionar canal del MUX SPI
 **************************************/
void select_channel_MUX(const uint8_t channel, const uint8_t PIN_SYNC_MUX);

/************************************
  Funcion para leer una NTC seleccionando un canal del ADG731
 **************************************/
float measure_temp(const int BETTA, const int To, const int Ro, const int Raux, const int Vcc, const uint8_t channel, const uint8_t AnalogInput, const uint8_t PIN_SYNC_MUX);
#endif
