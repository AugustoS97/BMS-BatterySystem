#include "Arduino.h"
#include "SPI.h"
#include <stdint.h>

/************************************
  Se selecciona un canal del MUX SPI
 **************************************/
/*
  Estructura del mensaje del SPI del MUX ADG731:

  |  DB7 (MSB)      | DB6             |  DB5            |       DB4       |      DB3        |      DB2        |       DB1       |  DB0 (LSB)    |
  |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|
  |   NOT(EN)       |     NOT(CS)     |       x         |      A4         |       A3        |     A2          |       A1        |      A0       |
  ENABLE  CS deben valer 0 durante la selección según la tabla de verdad
*/
void select_channel_MUX(const uint8_t channel, const uint8_t PIN_SYNC_MUX) {
  #ifdef SERIAL_DEBUG
    Serial.print("Canal selecc.: ");
    Serial.println(channel);
  #endif
  if (channel <= 32) {
    uint8_t msg = (0b00000000 | channel); //Se genera el mensaje con el valor de 1 en Enable y CSA y se concatena el canal a elegir
    #ifdef SERIAL_DEBUG
        Serial.print("Mensaje SPI es: ");
        Serial.println(msg, BIN); //Se imprime el mensaje a enviar por SPI en binario
    #endif
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE2)); 
    digitalWrite(PIN_SYNC_MUX, LOW); //Se pone a baja el CS del ADG731
    SPI.transfer(msg); //Se envia el mensaje por SPI
    digitalWrite(PIN_SYNC_MUX, HIGH);
    SPI.endTransaction();
  }
}

/************************************
  Funcion para leer una NTC seleccionando un canal del ADG731
 **************************************/
float measure_temp(const int BETTA, const int To, const int Ro, const int Raux, const int Vcc, const uint8_t channel, const uint8_t AnalogInput, const uint8_t PIN_SYNC_MUX) { //
  select_channel_MUX(channel, PIN_SYNC_MUX); //Se elige la entrada a medir llamando a la función anterior
  float v_medida = (float(Vcc)/1023.0)*float(analogRead(AnalogInput));
  float rntc = float(Raux)/((Vcc/v_medida)-1); //Vdividor = Vcc*R_NTC/(R_NTC+Ro)
  //rntc=4372;
  //float rntc = Ro * (Vcc / (analogRead(AnalogInput) * 5.0 / 1023.0) - 1); //Vdivisor=R_o/(R_o+R_NTC)
  float t_medida = (float(BETTA) / (log(rntc / float(Ro)) + (float(BETTA) / float(To))))-273; //Se obtiene en ºC
  #ifdef SERIAL_DEBUG
    Serial.println("Channel  | Tmedida ºC");
    Serial.print(channel);
    Serial.print("  | ");
    Serial.println(t_medida);
  #endif
  return t_medida;
}

/************************************
  Funcion que lee hasta 32 NTC y las almacena en un array pasado por referencia
 **************************************/
float measure_all_temp(float array_temp[], const uint8_t N_NTC, const int BETTA, const int To, const int Ro,const int Raux, const int Vcc, const uint8_t AnalogInput, const uint8_t PIN_SYNC_MUX) {
  float T_max = 0;
  for (uint8_t i = 0; i < N_NTC ; i++) {
    array_temp[i] = measure_temp(BETTA, To, Ro, Raux, Vcc, i,  AnalogInput, PIN_SYNC_MUX);
    if (array_temp[i] > T_max) {
      T_max = array_temp[i];
    }
  }
  return T_max;
}
