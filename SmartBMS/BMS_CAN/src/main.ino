/*
   AUTOR: AUGUSTO SAMUEL HERNÁNDEZ MARTÍN
   FECHA: 05/05/2021
   NOMBRE: SMART BMS
   VERSIÓN: V.1.0
   DESCRIPCIÓN: FIRMWARE DEL MICROCONTROLADOR ALOJADO EN LA PCB DEL CONFIGURADOR. SU FUNCIÓN ES CONFIGURAR EL BMS Y OBTENER SUS PARÁMETROS DESDE EL CAN BUS PARA DEVOLVERLOS POR SERIAL
*/
#include <Arduino.h>
#include <stdint.h>
#include "config.h"
#include "const.h"
#include "BMS_lib.h"
#include "ADG731_lib.h"
#include "mcp2515.h"


#include <LowPower.h>

MCP2515 mcp2515(9); //Se declara el objeto de la clase MCP2515


/*******************************************************
 * CAN STRUCT
 ******************************************************/
//Struct para almacenar mensaje CAN recibido de configuracion
struct can_frame can_msg;

//Struct para almacenar los mensajes CAN de la batería
struct can_frame canBatMsg1;
struct can_frame canBatMsg2;
struct can_frame canBatMsg3;

//Struct para almacenar los mensajes CAN de las Temperaturas
struct can_frame canTempMsg1;
struct can_frame canTempMsg2;
struct can_frame canTempMsg3;
struct can_frame canTempMsg4;

//Struct para almacenar el mensaje CAN de la corriente
struct can_frame canCurrentMsg;

struct can_frame warning; //Mensaje CAN de warning de baja tensión batería, batería en sobretensión o anomalía en Tª

struct can_frame canSOCMsg; //Mensaje CAN para el SOC

/********************************************************
 * BATTERY VALUES
 ********************************************************/
float UVBAT_THR = 12.00 ; //Tensión minima de la batería
float OVBAT_THR = 16.00 ; //Tensión maxima de la batería
float UV_THR = 1.0 ; //Tension minima de una celda
float OV_THR = 5.0; //Tension maxima de una celda
float MAX_VCELL_DIFF = 0.005; //Diferencia de tension maxima entre celdas
uint8_t TOTAL_CELL = 4; //Número de celdas totales
uint8_t NCELL_PARALLEL = 3; //Numero de celdas en paralelo
int16_t CURRENT_OFFSET = 0; //Offset del sensor de corriente (en mA)
uint8_t TSLEEP = 0; //tiempo de reposo entre ciclos

uint8_t BALANCING_TYPE = 0b11; //Tipo de balanceo. Bit0 balanceo durante carga. Bit1 balanceo durante descarga. Por defecto se activa el balanceo en carga y descarga.
bool force_bal_flag = false; //Flag que se activa cuando el balanceo se está forzando desde el programador. Con esto activo no se tiene en cuenta las condiciones de balanceo



/********************************************************
 * TEMPERATURE VALUES
 ********************************************************/
//Parametros de las NTC
uint8_t N_NTC = 32; //Numero de NTCs conectadas al BMS
float cell_temp [32]; //Array 1D con las temperaturas medidas


/********************************************************
 * ARRAYS
 ********************************************************/
uint16_t cell_codes[TOTAL_IC][12];//Array 2D de códigos de celda. Se indexan en forma de matriz con fila el IC que mide y columna la celda de la batería
/*
   Sigue el siguiente formado

  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
*/


uint8_t tx_cfg[TOTAL_IC][6];
/*
  El tx_cfg[][6] almacena los datos de configuración del LTC6804 que se van a escribir a los IC que están en Daisy Chain.
  La configuración de los LTC6804 se debe almacenar en bloques de 6 bytes.
  El array 2D es como se muestra a continuación.

  |  tx_cfg[0][0]| tx_cfg[0][1] |  tx_cfg[0][2]|  tx_cfg[0][3]|  tx_cfg[0][4]|  tx_cfg[0][5]| tx_cfg[1][0] |  tx_cfg[1][1]|  tx_cfg[1][2]|  .....    |
  |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
  |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |

*/

uint8_t rx_cfg[TOTAL_IC][8];
/*
  El rx_cfg[][8] almacena los datos que son leidos de los LTC6804-1 en Daisy-chain.
  Los datos de cada IC son almacenados en bloques de 8 bytes.
  A continuación se muestra la distribución del array 2D.

  |rx_config[0][0]|rx_config[0][1]|rx_config[0][2]|rx_config[0][3]|rx_config[0][4]|rx_config[0][5]|rx_config[0][6]  |rx_config[0][7] |rx_config[1][0]|rx_config[1][1]|  .....    |
  |---------------|---------------|---------------|---------------|---------------|---------------|-----------------|----------------|---------------|---------------|-----------|
  |IC1 CFGR0      |IC1 CFGR1      |IC1 CFGR2      |IC1 CFGR3      |IC1 CFGR4      |IC1 CFGR5      |IC1 PEC High     |IC1 PEC Low     |IC2 CFGR0      |IC2 CFGR1      |  .....    |

*/


uint16_t aux_codes[TOTAL_IC][6];
/*
  The GPIO codes will be stored in the aux_codes[][6] array in the following format:

  |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
  |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
  |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |
*/


/***********************************************************************
  SETUP
 ***********************************************************************/
void setup(){
  for (uint8_t i=0; i<TOTAL_IC; i++){ //Se inicializa el array de los V de las celdas
    for(uint8_t j=0 ; j<12 ; j++){
      cell_codes[i][j] = 65525;
    }
  }
  Serial.begin(115200);
  #ifdef SERIAL_DEBUG
    //Serial.begin(115200);
    print_config(tx_cfg, TOTAL_IC);
  #endif


  LTC6804_initialize();  //Inicializa el LTC6804
  init_cfg(tx_cfg, TOTAL_IC);      //Inicializa el array de configuración del 6804 a los valores por defecto

  read_eeprom_ltc(TOTAL_IC, tx_cfg); //Actualiza el array de config del LTC6804 con los valores almacenados en la EEPROM
  read_eeprom_atmega(UV_THR, OV_THR, N_NTC,TOTAL_CELL,UVBAT_THR, OVBAT_THR, MAX_VCELL_DIFF, BALANCING_TYPE, NCELL_PARALLEL, CURRENT_OFFSET, TSLEEP); //Se leen las configuraciones del ATMEGA desde EEPROM

  init_mcp2515(CAN_125KBPS, MCP_8MHZ, 0); //Se iniciliaza el MCP2515 para comunicacion CAN a 125 KBPS
  mcp2515.setConfigMode(); //Se configuran las mascaras del MCP2515 para aceptar solo mensajes con ID 0b000000xxxxxx
  mcp2515.setFilterMask(MCP2515::MASK0, false, 0b11111000000);
  mcp2515.setFilterMask(MCP2515::MASK1, false, 0b11111000000);
  mcp2515.setNormalMode();

  attachInterrupt(0, can_msg_rcv, FALLING); //Se configura interrupcion al recibir mensaje CAN de configuracion

  //Configuración de los mensajes CAN
  config_can_msg (canBatMsg1, BAT_MSG1_ID, 8); //Se indica el mensaje CAN, el ID (entero decimal) y el DLC (entero decimal)
  config_can_msg (canBatMsg2, BAT_MSG2_ID, 8);
  config_can_msg (canBatMsg3, BAT_MSG3_ID, 8);
  config_can_msg (canTempMsg1, TEMP_MSG1_ID, 8);
  config_can_msg (canTempMsg2, TEMP_MSG2_ID, 8);
  config_can_msg (canTempMsg3, TEMP_MSG3_ID, 8);
  config_can_msg (canTempMsg4, TEMP_MSG4_ID, 8);
  config_can_msg (warning, WARNING_ID, 1);
  config_can_msg (canCurrentMsg, CURRENT_MSG_ID, 4);
  config_can_msg (canSOCMsg, SOC_MSG_ID, 2);
  LTC6804_initialize();
}

/**********************************************************************
  LOOP
***********************************************************************/

void loop() {
  static unsigned long time_last_msg_bat = 0;
  static unsigned long time_last_msg_current = 0;
  static unsigned long time_last_msg_temp = 0;
  static unsigned long time_last_msg_soc = 0;
  static float T_max = 0.0;
  //Comienza la lectura y conversión de las tensiones leidas por el LTC6804
  read_cell_voltage (TOTAL_IC, TOTAL_CELL, tx_cfg, cell_codes);

  //Se convierten los valores de la tensión de las celdas a mensaje CAN
  cell_voltage_to_can_msg (cell_codes, TOTAL_IC, TOTAL_CELL, canBatMsg1, canBatMsg2, canBatMsg3);

  if (millis() > time_last_msg_bat + (TSLEEP * 15)){
    //Se envia el valor de las tensiones de celdas por CAN
    send_can_msg(canBatMsg1);

    if(TOTAL_CELL > 4){ //Si hay mas de 4 celdas se envia segundo y tercer mensaje CAN
      send_can_msg(canBatMsg2);
    }
    if(TOTAL_CELL >= 9){
      send_can_msg (canBatMsg3);
    }
    time_last_msg_bat = millis();
  }

  //Se mide la corriente y se envía el valor por CAN
  int32_t current = get_current (SAMPLESNUMBER, SENSIBILITY_CURRENT, PIN_CURRENT_SENSOR, CURRENT_OFFSET);
  current_to_can_msg (current, canCurrentMsg);
  if (millis() > time_last_msg_current + (TSLEEP *15)){
    send_can_msg(canCurrentMsg);
    time_last_msg_current = millis();
  }

  //Verificar y activar el balanceo de las celdas necesarias
  if(force_bal_flag == false){ //Si no se ha forzado el balanceo, se comprueba el tipo de balanceo
    switch(BALANCING_TYPE){
      case 0:{ //Caso de balanceo desactivado
        force_balancing (TOTAL_IC, tx_cfg, 0b00000000, true); //No se balanceo ninguna celda
        force_balancing (TOTAL_IC, tx_cfg, 0b00000000, false); //Se balancea celdas 1 a 8
      break;}
      case 1:{ //Balanceo solo durante la carga. Corriente medida es positiva
        if (current >=0){ //Si se esta cargando se balancea
          balancing(TOTAL_IC,  cell_codes, tx_cfg, MAX_VCELL_DIFF, TOTAL_CELL, OV_THR, UV_THR );
        }
        else{ //Si no se esta descargando, no se balancea
          force_balancing (TOTAL_IC, tx_cfg, 0b00000000, true); //Se dejan de balancear las celdas 1 a 8
          force_balancing (TOTAL_IC, tx_cfg, 0b00000000, false); //Se dejan de balancear las celdas 9 a 12
        }
        break;
      }
      case 2:{ //Balanceo solo durante la descarga
        if(current <=0){ //Balanceo solo durante la descarga. Corriente medida es negativa
          balancing(TOTAL_IC,  cell_codes, tx_cfg, MAX_VCELL_DIFF, TOTAL_CELL, OV_THR, UV_THR );
        }
        else{ //Si no se esta descargando, no se balancea
          force_balancing (TOTAL_IC, tx_cfg, 0b00000000, true); //Se dejan de balancear las celdas 1 a 8
          force_balancing (TOTAL_IC, tx_cfg, 0b00000000, false); //Se dejan de balancear las celdas 9 a 12
        }
        break;
      }
      case 3:{//Balanceo tanto durante la descarga como la carga
        balancing(TOTAL_IC,  cell_codes, tx_cfg, MAX_VCELL_DIFF, TOTAL_CELL, OV_THR, UV_THR );
        break;
      }
    }
  }

  //Calcular tensión total y generar aviso de tension o Temperatura
  float voltaje_total = calc_volt_total(cell_codes, TOTAL_IC, TOTAL_CELL);

  bool genera_warning = warning_msg(voltaje_total, UVBAT_THR, OVBAT_THR, T_max, warning);
  if (genera_warning) {
    //Se envia el aviso
    send_can_msg(warning);
  }

  //Se miden todas las temperaturas de las celdas y se guardan en cell_temp. Además se obtiene la T_max
  T_max = measure_all_temp(cell_temp, N_NTC, BETTA, To, Ro, Raux, Vcc, PIN_ANALOG_NTC, PIN_SYNC_MUX);

  //Se conviertes las temperaturas a mensajes CAN
  temp_to_can_msg(cell_temp, N_NTC, canTempMsg1, canTempMsg2, canTempMsg3, canTempMsg4);

  if (millis() > time_last_msg_temp + (TSLEEP *15)){
  //Se envian las Temperaturas de las celdas por CAN. Cada mensaje tiene 8 temperaturas
    send_can_msg(canTempMsg1); //Primeras 8 NTC
    if (N_NTC >8){ //NTC de 9 a 16
      send_can_msg(canTempMsg2);
    }
    if (N_NTC > 16){ //NTC de 17 a 24
      send_can_msg(canTempMsg3);
    }
    if (N_NTC >24){ //NTC de 24 a 32
      send_can_msg(canTempMsg4);
    }
    time_last_msg_temp = millis();
  }

  //Estimar SOH y SOC y enviarlos por CAN
  float SOC =  calculate_SOC(float(current/NCELL_PARALLEL), float(voltaje_total/TOTAL_CELL), internal_resistor);//Devuelve el SOC en porcentaje 0-100%
  SOC_can_msg(SOC, canSOCMsg);// Se codifica el SOC en un mensaje CAN de 2 bytes. Se envia el porcentaje multiplicado por 1000
  if (millis() > time_last_msg_soc + (TSLEEP *15)){
    send_can_msg(canSOCMsg);
    time_last_msg_soc = millis();
  }

  /*for (uint8_t i=0 ; i < TSLEEP; i++){
    LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_ON);
  }*/

} //FIN DEL LOOP


/************************************
  Se configura el mensaje CAN
 **************************************/
void config_can_msg (struct can_frame &can_msg, const uint8_t can_id, const uint8_t can_dlc) {
  can_msg.can_id = can_id;
  can_msg.can_dlc = can_dlc;
}


/************************************
 * Función que codifica la corriente en un mensaje CAN. Se dividen los 32 bits del float en 4 bytes del mensaje CAN
 ****************************************/
void current_to_can_msg ( int32_t current, struct can_frame &canCurrentMsg){
  canCurrentMsg.data[0] = (current) ; //Los 8 bits menos significados son el primer byte
  canCurrentMsg.data[1] = (current >> 8) ; //Los siguientes 8 bits del numero de 32 bits son el byte1
  canCurrentMsg.data[2] = (current >> 16);
  canCurrentMsg.data[3] = (current >> 24);
}


/************************************
  Se codifican las tensiones en mensajes CAN. Función
  Sigue este formato

  |  can_frame[7]      |    can_frame[6]     |    .....     |     can_frame[1]   |      can_frame[0]   |
  |--------------------|---------------------|--------------|--------------------|---------------------|
  | cell3 MSB          |     cell3 LSB       |    .....     |    cell0 MSB       |     cell0 LSB       |
  | cell_code[0][3] MSB|  cell_code[0][3] LSB|    .....     | cell_code[0][0] MSB|  cell_code[0][0] LSB|

 **************************************/
void cell_voltage_to_can_msg ( uint16_t cell_codes[][12], const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL, struct can_frame &canBatMsg1) {
  for (uint8_t i = 0; i < TOTAL_IC; i++) {
    for (uint8_t j = 0 ; j < TOTAL_CELL; j++) { //Cada medición ocupa 2 bytes (cell_codes son arrays de 16 bits)
      //Este algoritmo permite rellenar un mensaje de 8 bytes que contenga los 4 valores de 16 bits de las celdas. Con MSB los valores de la celda más alta
      //y LSB los valores LSB de la celda más baja
      canBatMsg1.data[2 * j] = cell_codes[i][j]; //Se toma la parte baja del registro de 16 bits
      canBatMsg1.data[2 * j + 1] = cell_codes[i][j] >> 8; //Se toma la parte alta del registro de 16 bits y se guarda en el byte siguiente del mnsje
    }
  }
}

void cell_voltage_to_can_msg (const uint16_t cell_codes[][12], const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL, struct can_frame &canBatMsg1,
                              struct can_frame &canBatMsg2) {
  for (uint8_t i = 0; i < TOTAL_IC; i++) {
    //Las primeras 4 celdas en el Mensaje 1 (canBatMsg1)
    for (uint8_t j = 0 ; j < 4; j++) { //Cada medición ocupa 2 bytes (cell_codes son arrays de 16 bits)
      //Este algoritmo permite rellenar un mensaje de 8 bytes que contenga los 4 valores de 16 bits de las celdas. Con MSB los valores de la celda más alta
      //y LSB los valores LSB de la celda más baja
      canBatMsg1.data[2 * j] = cell_codes[i][j]; //Se toma la parte baja del registro de 16 bits
      canBatMsg1.data[2 * j + 1] = cell_codes[i][j] >> 8; //Se toma la parte alta del registro de 16 bits y se guarda en el byte siguiente del mnsje
    }
  }
  for (uint8_t i = 0; i < TOTAL_IC; i++) {
    //Las celdas 4-7 en el Mensaje 2 (canBatMsg2)
    for (uint8_t j = 4 ; j < 8; j++) { //Cada medición ocupa 2 bytes (cell_codes son arrays de 16 bits)
      //Este algoritmo permite rellenar un mensaje de 8 bytes que contenga los 4 valores de 16 bits de las celdas. Con MSB los valores de la celda más alta
      //y LSB los valores LSB de la celda más baja
      canBatMsg2.data[2*(j - 4)] = cell_codes[i][j]; //Se toma la parte baja del registro de 16 bits
      canBatMsg2.data[2*(j - 4) + 1] = cell_codes[i][j] >> 8; //Se toma la parte alta del registro de 16 bits y se guarda en el byte siguiente del mnsje
    }
  }
}

void cell_voltage_to_can_msg (const uint16_t cell_codes[][12], const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL, struct can_frame &canBatMsg1,
                              struct can_frame &canBatMsg2, struct can_frame &canBatMsg3 ) {
  for (uint8_t i = 0; i < TOTAL_IC; i++) {
    //Las primeras 4 celdas en el Mensaje 1 (canBatMsg1)
    for (uint8_t j = 0 ; j < 4; j++) { //Cada medición ocupa 2 bytes (cell_codes son arrays de 16 bits)
      //Este algoritmo permite rellenar un mensaje de 8 bytes que contenga los 4 valores de 16 bits de las celdas. Con MSB los valores de la celda más alta
      //y LSB los valores LSB de la celda más baja
      canBatMsg1.data[2 * j] = cell_codes[i][j]; //Se toma la parte baja del registro de 16 bits
      canBatMsg1.data[(2 * j) + 1] = cell_codes[i][j] >> 8; //Se toma la parte alta del registro de 16 bits y se guarda en el byte siguiente del mnsje
    }
  }
  for (uint8_t i = 0; i < TOTAL_IC; i++) {
    //Las celdas 4-7 en el Mensaje 2 (canBatMsg2)
    for (uint8_t j = 4 ; j < 8; j++) { //Cada medición ocupa 2 bytes (cell_codes son arrays de 16 bits)
      //Este algoritmo permite rellenar un mensaje de 8 bytes que contenga los 4 valores de 16 bits de las celdas. Con MSB los valores de la celda más alta
      //y LSB los valores LSB de la celda más baja
      canBatMsg2.data[2*(j - 4)] = cell_codes[i][j]; //Se toma la parte baja del registro de 16 bits
      canBatMsg2.data[2*(j - 4) + 1] = cell_codes[i][j] >> 8; //Se toma la parte alta del registro de 16 bits y se guarda en el byte siguiente del mnsje
    }
  }
  for (uint8_t i = 0; i < TOTAL_IC; i++) {
    //Las celdas 4-7 en el Mensaje 2 (canBatMsg2)
    for (uint8_t j = 8 ; j < 12; j++) { //Cada medición ocupa 2 bytes (cell_codes son arrays de 16 bits)
      //Este algoritmo permite rellenar un mensaje de 8 bytes que contenga los 4 valores de 16 bits de las celdas. Con MSB los valores de la celda más alta
      //y LSB los valores LSB de la celda más baja
      canBatMsg3.data[2 * (j - 8)] = cell_codes[i][j]; //Se toma la parte baja del registro de 16 bits
      canBatMsg3.data[2 * (j - 8) + 1] = cell_codes[i][j] >> 8; //Se toma la parte alta del registro de 16 bits y se guarda en el byte siguiente del mnsje
    }
  }
}


/************************************
  Se verifica si generar un aviso de tensión baja de batería
 **************************************/
bool warning_msg(const float voltaje_total, const float umbral_vacio, const float umbral_cargado, const float T_max, struct can_frame warning) {
  static uint8_t estado_V = 1; //Se parte en el caso 1, de batería no llena pero no descargada
  static uint8_t estado_ant_V = 1;
  static uint8_t estado_T = 4; //Se parte en el caso 1, de batería no llena pero no descargada
  static uint8_t estado_ant_T = 4;

  //Caso de voltaje muy bajo
  if (voltaje_total < (umbral_vacio - 0.1)) { //Se restan 0.1 V para dejar una histéresis
    warning.data[0] = warning.data[0] | 0b00000001; //Bit 0 a 1 almacena info de bajo voltaje. A 0 no hay bajo V
    warning.data[0] = warning.data[0] & 0b11111101; //pone bit 1 a 0 para indicar batería no cargada. Valor dle bit 1 a 1 indica batería cargada
    estado_ant_V = estado_V;
    estado_V = 0;
  #ifdef SERIAL_DEBUG
      Serial.println("Vpack muy bajo");
  #endif
  }
  //Caso voltaje entre mínimo y máximo
  else if ((voltaje_total > (umbral_vacio + 0.1)) && voltaje_total < umbral_cargado) {
    warning.data[0] = warning.data[0] & 0b11111110; //Bit 0 a 0 para indicar que no hay voltaje demasiado bajo
    warning.data[0] = warning.data[0] & 0b11111101 ; //Bit 1 a 0 indica batería NO está cargada totalmente
    estado_ant_V = estado_V;
    estado_V = 1;
  #ifdef SERIAL_DEBUG
      Serial.println("Vpack OK");
  #endif
  }
  //Caso de batería llena
  else if (voltaje_total >= umbral_cargado) {
    warning.data[0] = warning.data[0] & 0b11111110; //Poner bit 1 a 0 para indicar batería NO tiene bajo voltaje
    warning.data[1] = warning.data[0] | 0b00000010; //Batería sobre-cargada. Poner bit 1 a valor 1 para indicar batería por encima del V máximo
    estado_ant_V = estado_V;
    estado_V = 2;
  #ifdef SERIAL_DEBUG
      Serial.println("Vpack superior al máximo");
  #endif
  }


  if (T_max > 60) { //si supera 60 se avisa de sobretemperatura
    warning.data[0] = warning.data[0] | 0b00000100;  //Bit 2 a 1 indica Temperatura máxima excedida
    estado_ant_T = estado_T;
    estado_T = 3;
  #ifdef SERIAL_DEBUG
      Serial.println("Temp. máx. excedida");
  #endif
  }
  else if (T_max < 50) { //Hasta no bajar a 50ºC no se quita aviso de sobretemperatura
    warning.data[0] = warning.data[0] & 0b11111011; //se pone a 0 el bit 2 al bajar la Tª nuevamente por debajo de 50ºC
    estado_ant_T = estado_T;
    estado_T = 4;
  #ifdef SERIAL_DEBUG
      Serial.println("Temp. adecuada");
  #endif
  }


  if ((estado_ant_V != estado_V) || (estado_ant_T != estado_T)) { //Si hay algún cambio de estado
  #ifdef SERIAL_DEBUG
      Serial.println("Cambios en las advertencias. Activando Warning");
      Serial.println("Estado anterior(V) | Estado actual(V)");
      Serial.print(estado_ant_V);
      Serial.print("     |   ");
      Serial.println(estado_V);
      Serial.println("Estado anterior(T) | Estado actual(T)");
      Serial.print(estado_ant_T);
      Serial.print("        |   ");
      Serial.println(estado_T);
  #endif
    return true; //Devuelve true si hay un cambio de estado para así enviar un nuevo mensaje warning
  }
  #ifdef SERIAL_DEBUG
    Serial.println("No hay cambio en advertencias. No warning");
  #endif
  return false; //Devuelve false si no hay cambio para así no enviar un nuevo warning
}

/************************************
  Se codifican las temperaturas en mensajes CAN
 **************************************/
void temp_to_can_msg(float array_temp[], const uint8_t N_NTC, struct can_frame &T_msg1, struct can_frame &T_msg2,
                     struct can_frame &T_msg3, struct can_frame &T_msg4) { //
  //Se estructuran siendo T[0] la situada en el LSByte y T[7] la situada en el MSByte del mensaje CAN
  for (uint8_t i = 0 ; i < 8 ; i++) { //8 primeras Temperaturas en el mensaje 1
    T_msg1.data[i] = uint8_t((array_temp[i] / 0.3) + 5);
  }
  T_msg1.data[0] = uint8_t((array_temp[1] / 0.3) + 5); //Se sobreescribe el valor del termistor 0 con el valor del siguiente.
  for (uint8_t i = 0 ; i < 8 ; i++) { //Siguientes 8 temperaturas
    T_msg2.data[i] = uint8_t((array_temp[i+8] / 0.3) + 5);
  }
  for (uint8_t i = 0 ; i < 8 ; i++) { //8 temperaturas siguientes
    T_msg3.data[i] = uint8_t((array_temp[i+16] / 0.3) + 5);
  }
  for (uint8_t i = 0 ; i < 8 ; i++) {
    T_msg4.data[i] = uint8_t((array_temp[i+24] / 0.3) + 5);
  }
}


/************************************
  codifica los SOH y SOC así como el estado
  carga/descarga en mensajes CAN
 **************************************/
void SOC_can_msg(float SOC, struct can_frame &SOC_msg) { // Se codifica el SOC en un mensaje CAN de 2 bytes. Se envia el porcentaje multiplicado por 100
  uint16_t _SOC = round(SOC * 100);
  SOC_msg.data[1] = uint8_t(_SOC >> 8);
  SOC_msg.data[0] = uint8_t(_SOC & 0b0000000011111111);
}


/************************************
  Envia el mensaje CAN deseado
 **************************************/
void send_can_msg(const struct can_frame &can_msg) { //
  #ifdef SERIAL_DEBUG
  Serial.println("Mensaje CAN enviado:");
  Serial.print(can_msg.can_id, HEX); // print ID
  Serial.print(" ");
  Serial.print(can_msg.can_dlc, HEX); // print DLC
  Serial.print(" ");
  for (int i = 0; i < can_msg.can_dlc; i++)  { // print the data
    Serial.print(can_msg.data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  #endif
  //init_mcp2515(CAN_125KBPS, MCP_8MHZ, 0); //Se iniciliaza el MCP2515 para comunicacion CAN a 125 KBPS
  //Serial.println("Enviando mensaje CAN");
  mcp2515.sendMessage(&can_msg);
  delay(200);
}


/************************************
  Inicializar módulo MCP2515
  0 para NormalMode
  1 para LoopbackMode
  2 para ListenOnlyMode
  Ejemplo de llamada init_mcp2515(CAN_125KBPS, MCP8MHZ, 0);
 **************************************/
void init_mcp2515(const CAN_SPEED canSpeed, CAN_CLOCK canClock, int mode) {
  mcp2515.reset();
  mcp2515.setBitrate(canSpeed, canClock);
  if (mode == 0) {
    mcp2515.setNormalMode();
  }
  else if (mode == 1) {
    mcp2515.setLoopbackMode();
  }
  else {
    mcp2515.setListenOnlyMode();
  }
}


void can_msg_rcv(){
  noInterrupts();
  if(mcp2515.readMessage(&can_msg) == MCP2515::ERROR_OK){
    uint8_t can_id = can_msg.can_id;
    //uint8_t can_dlc = can_msg.can_dlc;
    switch(can_id){ //Segun el ID recibido se guarda en la EEPROM en la posicion de memoria que corresponde
      case VUV_MSG_ID:{
        if(EEPROM.read(VUV_addr) != can_msg.data[0]){
          EEPROM.write(VUV_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        #ifdef SERIAL_DEBUG
        Serial.print("VUV:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      case VOV_MSG_ID:{
        if(EEPROM.read(VOV_addr) != can_msg.data[0]){
          EEPROM.write(VOV_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        #ifdef SERIAL_DEBUG
        Serial.print("VOV:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      case DCTO_MSG_ID:{
        if(EEPROM.read(DCTO_addr) != can_msg.data[0]){
         EEPROM.write(DCTO_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        #ifdef SERIAL_DEBUG
        Serial.print("DCTO:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      case NCELL_MSG_ID:{
        if(EEPROM.read(NCELL_addr) != can_msg.data[0]){
          EEPROM.write(NCELL_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        #ifdef SERIAL_DEBUG
        Serial.print("NCELL:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      case NCELL_PARALLEL_MSG_ID:{
        if(EEPROM.read(NCELL_PARALLEL_addr) != can_msg.data[0]){
          EEPROM.write(NCELL_PARALLEL_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        #ifdef SERIAL_DEBUG
        Serial.print("NCELL PARALEL:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      case CURRENT_OFFSET_MSG_ID:{
        if(EEPROM.read(CURRENT_OFFSET_addr) != can_msg.data[0]){
          EEPROM.write(CURRENT_OFFSET_addr, can_msg.data[0]); //Este mensaje CAN de configuración es de 2 bytes
        }
        if(EEPROM.read(CURRENT_OFFSET_addr2) != can_msg.data[1]){
          EEPROM.write(CURRENT_OFFSET_addr2, can_msg.data[1]); //Este byte lleva el signo del offset
        }
        #ifdef SERIAL_DEBUG
        //Serial.print("Current Offset:");
        //Serial.print(int16_t((uint16_t(uint8_t(can_msg.data[1]) << 8) | uint16_t(can_msg.data[0]))-32767), DEC);
        //Serial.println();
        //Serial.print(int16_t((uint16_t(uint8_t(EEPROM.read(CURRENT_OFFSET_addr2)) << 8) | uint16_t(EEPROM.read(CURRENT_OFFSET_addr)))-32767), DEC);
        #endif
        break;}
      case N_NTC_MSG_ID:{
        if(EEPROM.read(N_NTC_addr) != can_msg.data[0]){
         EEPROM.write(N_NTC_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        #ifdef SERIAL_DEBUG
        Serial.print("N_NTC:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      case TSLEEP_MSG_ID:{
        if(EEPROM.read(TSLEEP_addr) != can_msg.data[0]){
          EEPROM.write(TSLEEP_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        #ifdef SERIAL_DEBUG
        Serial.print("TSLEEP:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      case BAL1TO8_MSG_ID:{
        #ifdef SERIAL_DEBUG
        Serial.print("Balanceo celdas 1 a 8 forzadas:");
        Serial.println(can_msg.data[0], BIN);
        #endif
        if (can_msg.data[0] != 0){ //Si el mensaje no es 0, alguna celda se ha forzado a balancear
           force_bal_flag = true; //Se activa el flag de balanceo forzado
        }
        else{ //Si el mensaje es 0. Se ha desactivado el balanceo forzado
           force_bal_flag = false; //Se desactiva el flag de balanceo forzado
        }
        force_balancing (TOTAL_IC, tx_cfg, can_msg.data[0], true); //True indica que se balancee en el grupo 1 a 8
        break;}
      case BAL9TO12_MSG_ID:{
        #ifdef SERIAL_DEBUG
        Serial.print("Balanceo celdas 9 a 12 forzado: ");
        Serial.println(can_msg.data[0], BIN);
        #endif
        if (can_msg.data[0] != 0){ //Si el mensaje no es 0, alguna celda se ha forzado a balancear
           force_bal_flag = true; //Se activa el flag de balanceo forzado
        }
        else{ //Si el mensaje es 0. Se ha desactivado el balanceo forzado
           force_bal_flag = false; //Se desactiva el flag de balanceo forzado
        }
        force_balancing (TOTAL_IC, tx_cfg, can_msg.data[0], false); //True indica que se balancee en el grupo 1 a 8
        break;}
      case TYPE_BALANCING_MSG_ID:{
        if(EEPROM.read(BALANCING_TYPE_addr) != can_msg.data[0]){
          EEPROM.write(BALANCING_TYPE_addr, can_msg.data[0]);
        }
        break;
      }
      case ASK_CONFIG_MSG_ID:{ //Devuelve las configuraciones almacenadas en la EEPROM
        #ifdef SERIAL_DEBUG
        Serial.print("Parametros de config pedidos.");
        #endif
        can_msg.can_id = ANSWER_CONFIG_MSG_ID;
        can_msg.can_dlc = 8;
        can_msg.data[0] =   EEPROM.read(VUV_addr);
        can_msg.data[1] =   EEPROM.read(VOV_addr);
        can_msg.data[2] =   EEPROM.read(DCTO_addr);
        can_msg.data[3] =   EEPROM.read(NCELL_addr);
        can_msg.data[4] =   EEPROM.read(N_NTC_addr);
        can_msg.data[5] =   EEPROM.read(MAX_DIFF_CELL_addr);
        LTC6804_initialize();
        wakeup_sleep();
        uint8_t error = LTC6804_rdcfg(TOTAL_IC,rx_cfg); //Se lee la configuracion del LTC
        if (error != -1){
          can_msg.data[6] =   rx_cfg[0][4]; //En bytes 6 y 7 se envian celdas actualmente en balanceo
          can_msg.data[7] =   rx_cfg[0][5] & 0b00001111; //Podria usarse una funcion para obtener config
        }
        mcp2515.sendMessage(&can_msg); //Se envia el mensaje CAN con las configuraciones
        can_msg.can_id = ANSWER_CONFIG_MSG_2_ID;
        can_msg.can_dlc = 5;
        can_msg.data[0] =   EEPROM.read(NCELL_PARALLEL_addr);
        can_msg.data[1] =   EEPROM.read(CURRENT_OFFSET_addr2);
        can_msg.data[2] =   EEPROM.read(CURRENT_OFFSET_addr);
        can_msg.data[3] =   EEPROM.read(TSLEEP_addr);
        can_msg.data[4] =   EEPROM.read(BALANCING_TYPE_addr);
        mcp2515.sendMessage(&can_msg); //Se envia el mensaje CAN 2 con las configuraciones
        break;}
      case MAX_DIFF_CELL_MSG_ID:{
        if(EEPROM.read(MAX_DIFF_CELL_addr) != can_msg.data[0]){
          EEPROM.write(MAX_DIFF_CELL_addr, can_msg.data[0]); //Todos los mensajes CAN de configuracion son de 1 byte
        }
        MAX_VCELL_DIFF = float( can_msg.data[0] * 0.001); //Se guarda el valor de maxima_diferencia de tension
        #ifdef SERIAL_DEBUG
        Serial.print("Máx. diferencia mV entre celdas:");
        Serial.println(can_msg.data[0],BIN);
        #endif
        break;}
      default:
        break;
    }
  }
  read_eeprom_ltc (TOTAL_IC, tx_cfg); //Se actualiza el array de config del LTC leyendo los parametros de la EEPROM
  read_eeprom_atmega(UV_THR, OV_THR, N_NTC,TOTAL_CELL,UVBAT_THR, OVBAT_THR, MAX_VCELL_DIFF, BALANCING_TYPE, NCELL_PARALLEL, CURRENT_OFFSET, TSLEEP); //Se actualizan los valores de config del ATMega
  LTC6804_initialize();
  LTC6804_wrcfg(TOTAL_IC, tx_cfg); //Se actualiza la configuracion del LTC
  delay(10);
  #ifdef SERIAL_DEBUG
    print_config(tx_cfg, TOTAL_IC);
  #endif
  interrupts();
}
