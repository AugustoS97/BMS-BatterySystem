#include <Arduino.h>
#include "config.h"
#include "serial_lib.h"
#include "mcp2515.h"

String inReadString = "";

MCP2515 mcp2515(9);

struct can_frame can_msg_in;
struct can_frame can_msg_out;


void can_msg_rcv(){
  noInterrupts();
  if(mcp2515.readMessage(&can_msg_in) == MCP2515::ERROR_OK){
    //Serial.print("Mensaje recibido Correctamente. ID:");
    //Serial.println(can_msg_in.can_id, DEC);
    switch (can_msg_in.can_id){
    case ANSWER_CONFIG_MSG_ID:{ //Recibir configuraciones del BMS
      uint8_t aux = can_msg_in.data[0];
      Serial.print(VUV_SERIAL_ID);
      Serial.println(aux, DEC); //Envia por serie el valor de VUV almacenado
      aux = can_msg_in.data[1];
      Serial.print(VOV_SERIAL_ID);
      Serial.println (aux, DEC); //Envia por serie el valor de VOV almacenado
      aux = can_msg_in.data[2];
      Serial.print(DCTO_SERIAL_ID);
      Serial.println(aux, DEC); //Envia por serie el valor de DCTO almacenado
      aux = can_msg_in.data[3];
      Serial.print(NCELLS_SERIAL_ID);
      Serial.println(aux, DEC); //Envia por serie el numero de celdas almacenado (NCELL)
      aux = can_msg_in.data[4];
      Serial.print(NTEMPS_SERIAL_ID);
      Serial.println (aux, DEC); //Envia por serie el numero de NTC (N_NTC)
      aux = can_msg_in.data[5];
      Serial.print(MAX_DIFF_CELL_SERIAL_ID);
      Serial.println(aux, DEC); //Envia por serie el Valor de máximo V de diferencia (en mV) entre celdas
      aux = can_msg_in.data[6];
      Serial.print(BAL1_8_SERIAL_ID);
      Serial.println(aux, DEC); //Devuelve las celdas que están balanceandose en ese instante de la 1 a la 8
      aux = can_msg_in.data[7];
      Serial.print(BAL9_12_SERIAL_ID);
      Serial.println(aux, DEC); //Devuelve las celdas entre la 9 y la 12 que se estan balanceando (4 primeros bits)
      break;}
    case ANSWER_CONFIG_MSG_2_ID:{ //Recibir 2 mensaje de configuraciones del BMS
      uint8_t aux = can_msg_in.data[0];
      Serial.print(NCELLS_PARALLEL_SERIAL_ID);
      Serial.println(aux, DEC); //Envia por serie el valor de numero de celdas en paralelo
      int16_t current_offset = int16_t(int16_t(uint16_t((can_msg_in.data[1] << 8)) | uint16_t(can_msg_in.data[2]) )- 32767); //Se reconstruye el offset de corriente desde 2 bytes
      Serial.print(CURRENT_OFFSET_SERIAL_ID);
      Serial.println(current_offset, DEC); //Envia por serie el offset
      aux = can_msg_in.data[3];
      Serial.print(TSLEEP_SERIAL_ID);
      Serial.println(aux, DEC); //Envia por serie el valor del factor que multiplica a 15 ms al tsleep
      aux = can_msg_in.data[4];
      Serial.print(TYPE_BALANCING_SERIAL_ID);
      Serial.println(aux, DEC); //Envia por serie el tipo de balanceo
      break;}

    case BAT_MSG1_ID: //Se recibe el mensaje CAN de las celdas 1 a 4
      for (int i=0; i<4 ; i++){
        Serial.print(BAT_MSG1_SERIAL_ID);
        Serial.print(i+1);
        uint16_t aux = (can_msg_in.data[(2*i)+1] << 8) | can_msg_in.data[2*i];
        Serial.println(aux, DEC);
      }
      break;
    case BAT_MSG2_ID://Se recibe el mensaje CAN de las celdas 5 a 8
      for (int i=0; i<4 ; i++){
        Serial.print(BAT_MSG2_SERIAL_ID);
        Serial.print(i+1);
        uint16_t aux = (can_msg_in.data[2*i+1] << 8) | can_msg_in.data[2*i];
        Serial.println(aux, DEC);
      }
      break;
    case BAT_MSG3_ID:
      for (int i=0; i<4 ; i++){
        Serial.print(BAT_MSG3_SERIAL_ID);
        Serial.print(i+1);
        uint16_t aux = (can_msg_in.data[2*i+1] << 8) | can_msg_in.data[2*i];
        Serial.println(aux, DEC);
      }
      break;
    case TEMP_MSG1_ID: //Se recibe por CAN las 8 primeras temperaturas
      for (int i=1; i<9; i++){
        Serial.print(TEMP_MSG_SERIAL_ID);
        Serial.print("0");
        Serial.print(i, DEC);
        uint8_t aux = can_msg_in.data[i-1];
        Serial.println(aux, DEC); //Se envia M01 y el valor para la NTC0, M02 para NTC num 2...
      }
      break;
    case TEMP_MSG2_ID: //Temperaturas de las NTC 9 a 16
      Serial.print(TEMP_MSG_SERIAL_ID);
      Serial.print("09");
      Serial.println(can_msg_in.data[0], DEC); //Se envia P09 para la NTC numero 9
      for (int i=10; i<17; i++){
        Serial.print("P");
        Serial.print(i, DEC);
        uint8_t aux = can_msg_in.data[i-9];
        Serial.println(aux, DEC); //Se envia M10 y el valor para la NTC10, M11 para NTC num 11...
      }
      break;
    case TEMP_MSG3_ID: //Temperaturas de la NTC 17 a 25
      for (int i=17; i<25; i++){
        Serial.print(TEMP_MSG_SERIAL_ID);
        Serial.print(i, DEC);
        uint8_t aux = can_msg_in.data[i-17]; //Se envia M17 y el valor para la NTC17, M18 para NTC num 18...
        Serial.println(aux, DEC);
      }
      break;
    case TEMP_MSG4_ID:
    //Serial.println("Recibido 25 a 32");
      for (int i=25; i<33; i++){
        Serial.print(TEMP_MSG_SERIAL_ID);
        Serial.print(i, DEC);
        uint8_t aux = can_msg_in.data[i-25]; //Se envia M25 y el valor para la NTC25, M32 para NTC num 32...
        Serial.println(aux, DEC);
      }
      break;
    case CURRENT_MSG_ID:{ //Corriente medida de la batería
      int32_t current = 0;
      current = ((uint32_t(can_msg_in.data[3]) << 24)) | ((uint32_t(can_msg_in.data[2]) << 16)) | ((uint32_t(can_msg_in.data[1]) << 8)) | (uint32_t(can_msg_in.data[0])) ;
      Serial.print(CURRENT_MSG_SERIAL_ID);
      Serial.println(current, DEC); //Se envia la corriente en mA
    }
    case SOC_MSG_ID:{ // SOC del pack
      Serial.print(SOC_MSG_SERIAL_ID);
      uint16_t SOC = (uint16_t(can_msg_in.data[1]) << 8) | can_msg_in.data[0];
      Serial.println(SOC, DEC);
      break;}
    case SOH_MSG_ID:{ //SOH del pack
      Serial.print(SOH_MSG_SERIAL_ID);
      uint8_t aux = can_msg_in.data[0];
      Serial.println(aux, DEC);
      //Serial.println("U"+can_msg_in.data[0]); //Se envia el SOH que es un numero de 1 byte
      break;}
  } //Fin del Switch
  }
  interrupts();
}
void setup() {
  attachInterrupt(0, can_msg_rcv, FALLING); //Se configura interrupcion al recibir mensaje CAN
  Serial.begin(115200);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void loop() {
  //Serial.println("Hello");
  inReadString = serial_event(); //Se lee puerto serial y se almacena lo leido
  char ID_in = char(inReadString[0]); //Se obtiene el ID que es el byte 0 del String leido por serial
  int config_value = int(get_data(inReadString)); //Se obtiene el valor del parametro y se  almacena en 1 byte
  switch (ID_in){
    case VUV_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de VUV. Valor: ");
        Serial.print(config_value, BIN);
        Serial.print(" = ");
        Serial.println(config_value*0.02);
      #endif
      can_msg_out.can_id = VUV_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case VOV_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de VOV. Valor: ");
        Serial.print(config_value, BIN);
        Serial.print(" = ");
        Serial.println(config_value*0.02);
      #endif
      can_msg_out.can_id = VOV_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case DCTO_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de DCTO. Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = DCTO_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case NCELLS_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de N_CELDAS. Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = NCELL_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case NTEMPS_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de N SENSORES Temp. Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = N_NTC_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case TSLEEP_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de TSLEEP. Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = TSLEEP_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value); //Factor que se multiplica por 15ms para el tiempo que duerme el BMS
      mcp2515.sendMessage(&can_msg_out);
      break;
    case BAL1_8_SERIAL_ID:
    #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de Balanceo 1-8. Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = BAL1TO8_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case BAL9_12_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de Balanceo 9-12. Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = BAL9TO12_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case MAX_DIFF_CELL_SERIAL_ID:
    #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de Máxima V de diferencia entre celdas (mV) . Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = MAX_DIFF_CELL_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case TYPE_BALANCING_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de configuración del tipo de balanceo:");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = TYPE_BALANCING_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;

    case NCELLS_PARALLEL_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de configuración del número de celdas en paralelo:");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = NCELL_PARALLEL_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    case CURRENT_OFFSET_SERIAL_ID:
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de configuración del offset del sensor corriente:");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = CURRENT_OFFSET_MSG_ID;
      can_msg_out.can_dlc = 2;
      can_msg_out.data[0] = uint8_t(config_value+32767); //Se toman los 8 bits menos significativos
      can_msg_out.data[1] = uint8_t((config_value+32767) >> 8); //Se toman los 8 bits mas significativos
      mcp2515.sendMessage(&can_msg_out);
      Serial.println((can_msg_out.data[0]|(can_msg_out.data[1]<<8))-32767);
      delay(1000);
      break;

    case ASK_CONFIG_SERIAL_ID: //Para pedir todos los valores de config. Escribir Z255
      #ifdef SERIAL_DEBUG
        Serial.print("Mensaje de pedir Config Actual. Valor: ");
        Serial.println(config_value, BIN);
      #endif
      can_msg_out.can_id = ASK_CONFIG_MSG_ID;
      can_msg_out.can_dlc = 1;
      can_msg_out.data[0] = uint8_t(config_value);
      mcp2515.sendMessage(&can_msg_out);
      break;
    default:
      break;
  } //Fin del Switch

}
