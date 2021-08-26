#include "BMS_lib.h"


/***********************************
 * Función para medir la corriente  en mA
 ************************************/
 int32_t get_current (const int samples_number, const float sensibility_current, const uint8_t PIN_CURRENT_SENSOR, const int16_t CURRENT_OFFSET){
  float current_sum = 0;
  for (int i=0; i<samples_number; i++){
    float voltage = analogRead(PIN_CURRENT_SENSOR) * 5.0 /1023.0;
    current_sum += (voltage -2.5)/sensibility_current; //V = 2.5 + K*I
  }
  int32_t current = round((current_sum/samples_number)*1000);


  //Serial.print("Valor leido: ");
  //Serial.println(current);
  //Serial.print("Current offset =");
  //Serial.println(CURRENT_OFFSET);

  return(current + CURRENT_OFFSET); //Devuelve la corriente medida en mA aplicandole el offset
}


/************************************
 * Algortimo de balanceo
 ***********************************^*/
 void balancing(const uint8_t TOTAL_IC,  uint16_t cell_codes[][12], uint8_t tx_cfg[][6], float max_difference, uint8_t TOTAL_CELL, float VOV_THR, float VUV_THR ){
  float V_min=cell_codes[0][0]*0.0001;
  float V_max = cell_codes[0][0]*0.0001;
  static uint16_t cell_to_balance = 0b0000000000000000;

  //Se obtiene V_min y V_max
  for (int i=0; i< TOTAL_IC; i++){
    for(int j=0; j<TOTAL_CELL; j++){
      if(float(cell_codes[i][j]* 0.0001) <= V_min){//Según datasheet el valor medido se multiplica por 100uV
        V_min = float(cell_codes[i][j]* 0.0001);
      }
      if(float(cell_codes[i][j]* 0.0001) >= V_max){
        V_max = float(cell_codes[i][j]* 0.0001);
      }
    }
  }

  //Se busca que celdas balancear
  for (int i=0; i< TOTAL_IC; i++){//Se obtiene V_min y V_max
    for(int j=0; j<TOTAL_CELL; j++){
      float cell_volt = cell_codes[i][j]*0.0001; //Voltaje de la celda j en Voltios
        if((((cell_volt - V_min) >= (max_difference))&(cell_volt > VUV_THR))||(cell_volt >= VOV_THR)){
       //Si la celda presenta una diferencia con la de menor V superior a la máx. diferencia o supera la máxima tensión, se balancea.
        cell_to_balance = (cell_to_balance | (0b1 << j));
       }
       else{
        if((cell_volt - V_min) <= (max_difference-0.0003)&&(cell_volt < (VOV_THR - 2*max_difference))){
          //Si la celda ha bajado del umbral de sobrevoltaje o está por debajo de la tensión mínima o dentro del rango de máxima diferencia, se para el balanceo
          cell_to_balance = (cell_to_balance & ~(uint16_t(0b1 << j))); //Pongo a 0 el bit de la celda que no se debe balancear
        }
       }
    }
  }

  //Se balancean las celdas indicadas
   uint8_t cell_to_bal_A = cell_to_balance & 0b0000000011111111; //Se obtiene el byte de las celdas 1 a 8
   uint8_t cell_to_bal_B = ((cell_to_balance >> 8) & 0b0000000000001111); //Se obtiene el byte de las celdas 9 a 12
   force_balancing (TOTAL_IC, tx_cfg, cell_to_bal_A, true); //Se balancea celdas 1 a 8
   force_balancing (TOTAL_IC, tx_cfg, cell_to_bal_B, false); //Se balancea celdas 9 a 12
   LTC6804_initialize();
   LTC6804_wrcfg(TOTAL_IC, tx_cfg); //Se actualiza la configuracion del LTC con la configuración de balanceo
 }


 /************************************
 * Forzar balanceo de las celdas indicadas
 ***********************************/
 void force_balancing (const uint8_t TOTAL_IC, uint8_t tx_cfg[][6], uint8_t cells_to_balance, bool group){ //Si group= true. Se balancea grupo celdas 1 a 8
  //Si group = false. Balancea grupo de celdas 9 a 12
  if( group){
    for (uint8_t i=0 ; i< TOTAL_IC; i++){
      tx_cfg[i][4] = cells_to_balance; //Se escriben los 8 bit de las celdas a balancear
    }
  }
  else{
    for (uint8_t i=0 ; i< TOTAL_IC; i++){
      tx_cfg[i][5] = ((tx_cfg[i][5])& 0b11110000) | (cells_to_balance & 0b00001111); //Se escribe los 4 bits de las celdas a balancer en los bit 0 a 3.
    }
  }
 }


 /************************************
  Lectura de las configuraciones para el LTC6804 desde la EEPROM
  del mmicrocontrolador
  **********************************/
void read_eeprom_ltc (const uint8_t TOTAL_IC, uint8_t tx_cfg[][6]){
  //La funcion actualiza los valores del array de configuracion tx_cfg[][6]
  uint16_t VUV = EEPROM.read(VUV_addr)*(0.02)*(1/0.0016) -1;
  /*Se convierte el valor de umbral de bajo-voltaje desde la codificacion
  con la que se almacena en la EEPROM, VUV(real)=VUV_eeprom * 0.02 al valor
  que necesita el LTC (VUV_real=(VUV+1)*16*100uV)*/
  uint16_t VOV = EEPROM.read(VOV_addr)*(0.02)*(1/0.0016) -1; //Se procede igual con
  //el umbral de sobre-voltaje
  for (uint8_t i=0 ; i< TOTAL_IC; i++){
    //tx_cfg[i][0] = EEPROM.read(CFGR0_addr);
    tx_cfg[i][1] =  VUV & (0b11111111);
    tx_cfg[i][2] = (((VUV>>8) & (0b00001111)) | ((VOV & 0b00001111)<< 4));
    tx_cfg[i][3] = ((VOV >> 4) & (0b11111111));
    tx_cfg[i][5] = ((tx_cfg[i][5] & 0b00001111) |(EEPROM.read(DCTO_addr)<< 4));
    //Se ponen a 0 los 4 bits del Discharge TimeOut y se carga el valor que se tiene
    //almacenado de Discharge TimeOut
  }
}



/************************************
  Lectura de las configuraciones del ATMega desde la EEPROM
  Umbrales de voltaje celda inferior y superior, num de NTC, numero de celdas
  y Umbrales de voltaje de la bateria inferior y superior
*************************************/
void read_eeprom_atmega(float &UV_THR, float &OV_THR, uint8_t &N_NTC,
    uint8_t &TOTAL_CELL, float &UVBAT_THR, float &OVBAT_THR, float &MAX_VCELL_DIFF, uint8_t &BALANCING_TYPE, uint8_t &NCELL_PARALLEL, int16_t &CURRENT_OFFSET , uint8_t &TSLEEP){
  uint8_t value = uint8_t(EEPROM.read(NCELL_addr));
  if( value <= 12){
    TOTAL_CELL = value;
  }
  value = EEPROM.read(VUV_addr);
  if (value <= 254){
     UV_THR = float(EEPROM.read(VUV_addr))*0.02; //Se obtiene el valor umbral mínimo tension de la celda
  }
  OV_THR = float(EEPROM.read(VOV_addr))*0.02; //Se obtiene el valor maximo de V de la celda
  value = uint8_t(EEPROM.read(N_NTC_addr));
  if( value <= 32){
      N_NTC = value ;
  }
  UVBAT_THR= UV_THR * TOTAL_CELL; //voltaje minimo de toda la bateria
  OVBAT_THR = OV_THR * TOTAL_CELL; //voltaje maximo de toda la bateria
  MAX_VCELL_DIFF = float(EEPROM.read(MAX_DIFF_CELL_addr)*0.001);
  value = uint8_t(EEPROM.read(BALANCING_TYPE_addr)); //Se obtiene la configuración del balanceo
  if(value <=3){
    BALANCING_TYPE = value;
  }
  if (value <= 99){
    NCELL_PARALLEL = value; //Se obtiene el valor de numero de celdas paralelo
  }
  CURRENT_OFFSET = int16_t((uint16_t(uint8_t(EEPROM.read(CURRENT_OFFSET_addr2)) << 8) | uint16_t(EEPROM.read(CURRENT_OFFSET_addr)))-32767);//Se obtiene el offset de la corriente
  value = uint8_t(EEPROM.read(TSLEEP_addr));
    if(value <= 220){
      TSLEEP = value;
    }
    else{
      TSLEEP = 0;
    }

}


/************************************
  Se comienza la conversión ADC de las tensiones
  de las celdas
 **************************************/
void start_cell_voltage_ADC(const uint8_t TOTAL_IC, uint8_t tx_cfg[][6]) {
  #ifdef SERIAL_DEBUG
  print_config(tx_cfg, TOTAL_IC);
  Serial.println("Iniciando conv. ADC de Vcell");
  #endif
  LTC6804_initialize();
  wakeup_sleep(); //Despierta el IC del modo sleep
  LTC6804_wrcfg(TOTAL_IC, tx_cfg); //configura el IC
  LTC6804_adcv(); //comienza la conversión ADC de las entradas de voltaje (Cx) del IC
  delay(3);
}


/************************************
  Se leen las tensiones de las celdas
 **************************************/
void read_cell_voltage (const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL, uint8_t tx_cfg[][6], uint16_t cell_codes[][12]) {
  start_cell_voltage_ADC(TOTAL_IC, tx_cfg); //Realiza la conversión ADC de las tensiones
  uint16_t cell_codes_aux[TOTAL_IC][12]; //Se crea un array auxiliar
  for (int i = 0; i < TOTAL_IC; i++) { //Se almacena la lectura anterior en el array auxiliar
    for (int j = 0; j < TOTAL_CELL; j++) {
      cell_codes_aux[i][j] = cell_codes[i][j];
    }
  }
  int8_t error; //Se crea la variable que almacena el estado de error del CRC
  error = LTC6804_rdcv(0, TOTAL_IC, cell_codes); // Se lee y hace un parses de las tensiones de las celdas
  if (error == -1) { //Si el error vale -1 es que hubo un error con el checkeo del CRC
  #ifdef SERIAL_DEBUG
    Serial.println("Error en CRC Vcell");
  #endif
    for (int i = 0; i < TOTAL_IC; i++) { //Si los valores recibidos no son correctos, se recuperan los valores de la lectura anterior
      for (int j = 0; j < TOTAL_CELL; j++) {
        cell_codes[i][j] = cell_codes_aux[i][j];
      }
    }
  }
  else { //Si no hay error con el CRC se procede a mostrar los valores medidos
  #ifdef SERIAL_DEBUG
    print_cells(TOTAL_IC, cell_codes); //Se muestran los valores de tensión medidos
  #endif
  }
}



/************************************
  Se calcula la tensión total de la batería
 **************************************/
float calc_volt_total(const uint16_t cell_codes[][12], const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL) {
  float voltage_total = 0;
  for (uint8_t i = 0; i < TOTAL_IC ; i++) {
    for (uint8_t j = 0 ; j < TOTAL_CELL; j++) {
      voltage_total += (cell_codes[i][j] * 0.0001); //Se pasa la tensión a decimal y se suman
    }
  }
  return voltage_total;
}




/************************************
  Se calcula el SOC del pack de baterías
 **************************************/
float calculate_SOC(int current, float voltage, float resistor) { //Devuelve el SOC en porcentaje 0-100%
  float Voc = voltage - float(current)/1000.0 * resistor; //Corriente es positiva cuando se carga bateria. Voc=Vo-I*R
  //Se evalua la expresion del voltaje para obtener el SOC
  float SOC = (-0.4203*pow(Voc, 4) + 5.6962*pow(Voc,3) - 28.2351*pow(Voc,2) + 61.1863*Voc - 49.1338)*100.0;
  //Serial.print("SOC:");
  //Serial.println(SOC);
  if (SOC > 100.0){
    SOC = 100;
  }
  else if (SOC <= 0.0){
    SOC = 0.0;
  }
  return SOC;
}



/************************************
  Se calcula el SOH del pack de baterías
 **************************************/
void calculate_SOH() { //

}


/************************************
  Se inicializa el paquete de datos a los valores de configuración
 **************************************/
void init_cfg(uint8_t tx_cfg[][6], const uint8_t TOTAL_IC) {
  for (int i = 0; i < TOTAL_IC; i++) {
    tx_cfg[i][0] = 0xFE;
    tx_cfg[i][1] = 0x00 ;
    tx_cfg[i][2] = 0x00 ;
    tx_cfg[i][3] = 0x00 ;
    tx_cfg[i][4] = 0x00 ;
    tx_cfg[i][5] = 0x00 ;
  }
}


/*************************************************************
  Imprime los voltajes de las Celdas Por puerto Serial
 *************************************************************/
void print_cells(const uint8_t TOTAL_IC, uint16_t cell_codes[][12]) {
  //void print_cells(const uint8_t TOTAL_IC, uint16_t *cell_codes){
  Serial.println("Voltaje de las Celdas");
  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++) {
    Serial.print(" IC ");
    Serial.print(current_ic + 1, DEC);
    for (int cell = 0; cell < 12; cell++) {
      Serial.print(" C");
      Serial.print(cell + 1, DEC);
      Serial.print(":");
      Serial.print(cell_codes[current_ic][cell] * 0.0001, 4);
      Serial.print(",");
    }
    Serial.println();
  }
  Serial.println();
}


/*****************************************************************************
  Imprime por puerto Serial los Voltajes de los GPIO leidos y el valor de Vref2
 *****************************************************************************/
void print_aux(uint16_t aux_codes[][6], const uint8_t TOTAL_IC) {
  //void print_aux(uint16_t *aux_codes){

  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++) {
    Serial.print(" IC ");
    Serial.print(current_ic + 1, DEC);
    for (int i = 0; i < 5; i++) {
      Serial.print(" GPIO-");
      Serial.print(i + 1, DEC);
      Serial.print(":");
      Serial.print(aux_codes[current_ic][i] * 0.0001, 4);
      Serial.print(",");
    }
    Serial.print(" Vref2");
    Serial.print(":");
    Serial.print(aux_codes[current_ic][5] * 0.0001, 4);
    Serial.println();
  }
  Serial.println();
}



/*******************************************************************************
  Imprime por Puerto Serial la Configuración que va a escribirse en el LTC6804
 ********************************************************************************/
void print_config(uint8_t tx_cfg[][6], const uint8_t TOTAL_IC) {
  int cfg_pec;

  Serial.println("Configuración: ");
  for (int current_ic = 0; current_ic < TOTAL_IC; current_ic++) {
    Serial.print(" IC ");
    Serial.print(current_ic + 1, DEC);
    Serial.print(": ");
    Serial.print("0x");
    serial_print_hex(tx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][5]);
    Serial.print(", CRC : 0x");
    cfg_pec = pec15_calc(6, &tx_cfg[current_ic][0]);
    serial_print_hex((uint8_t)(cfg_pec >> 8));
    Serial.print(", 0x");
    serial_print_hex((uint8_t)(cfg_pec));
    Serial.println();
  }
  Serial.println();
}

/******************************************************************
  Imprime por Puerto Serial la configuración que se ha leido del LTC6804
 *******************************************************************/
void print_rxconfig(uint8_t rx_cfg[][8], const uint8_t TOTAL_IC) {
  Serial.println("Configuración recibida: ");
  for (int current_ic = 0; current_ic < TOTAL_IC; current_ic++) {
    Serial.print(" IC ");
    Serial.print(current_ic + 1, DEC);
    Serial.print(": 0x");
    serial_print_hex(rx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][5]);
    Serial.print(", CRC recibido: 0x");
    serial_print_hex(rx_cfg[current_ic][6]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][7]);
    Serial.println();
  }
  Serial.println();
}




/******************************************************************
  Imprime por Puerto Serial en Hexadecimal
 *******************************************************************/
void serial_print_hex(uint8_t data) {
  if (data < 16) {
    Serial.print("0");
    Serial.print((byte)data, HEX);
  }
  else
    Serial.print((byte)data, HEX);
}
