#include "serial_lib.h"
#include <Arduino.h>

/******************************************************************************
  FUNCIÓN PARA LEER EL SERIAL HASTA UN SALTO DE LINEA Y DEVOLVERLO COMO STRING
******************************************************************************/
String serial_event(){
  String inputString = "";
  inputString.reserve(10); //Se reservan 200 bytes
  while(Serial.available()){
    char inChar = (char)Serial.read();
    inputString += inChar;
    if(inChar == '\n'){
      return inputString;
    }
  }
}

/******************************************************************************
  FUNCIÓN PARA RECIBIR UN STRING CON ID (1 CARACTER) Y OBTENER EL VALOR DE CONFIGURACION
******************************************************************************/
int get_data (String serial_msg){
  String data = "";
  for (unsigned int i=1; i<serial_msg.length()-1; i++){ //comienza en 1 para eliminar el ID y acaba en length -2 para evitar el '\n'
    data += char(serial_msg[i]); //Se anida en el string el siguiente caracter
  }
  return data.toInt();
}
