# BMS
Firmware del nodo BMS que monitoriza la tensión, la temperatura de las celdas, la corriente
y realiza el balanceo de las celdas, cuando la diferencia entre celdas supera un valor establecido.
Transmite los parámetros medidos por CANBUS y puede ser configurado mediante CANBUS.

# Mensajes CAN
A continuación se indican los mensajes CAN que son intercambiados entre el nodo BMS y el resto de nodos.

### Warning. Mensaje CAN
- Este mensaje es envíado por el BMS cuando hay una sobretemperatura o una tensión demasiado baja o alta. Su ID = 0x00 y está formado por 1 byte. Los bits a 1 indican las alertas que están activas:
  - ID = 0x00 (WARNING_ID)
  - DLC = 1 byte
  - data[0] = x;x;x;x;x;OverTemp;NotCharged;UnderVoltage

### Configurar valor VUV. Mensaje CAN
- Este mensaje configura el valor de tensión mínima de la celda en el BMS.
  - El ID en hexadecimal del mensaje CAN es ID=0x02. (VUV_MSG_ID)
  - DLC = 1 byte
  - Valor (Voltios) = data[0] * 0,02 V
  - Se almacena el valor en la dirección de memoria 0x02 de la EEPROM (VUV_addr)

### Configurar valor VOV. Mensaje CAN
- Este mensaje configura el valor de tensión máxima de la celda. en el BMS
  - El ID en hexadecimal del mensaje CAN es ID=0x03. (VOV_MSG_ID)
  - DLC = 1 byte
  - Valor (Voltios) = data[0] * 0,02 V
  - Se almacena el valor en la dirección de memoria 0x03 de la EEPROM (VOV_addr)

### Configurar valor DCTO. Mensaje CAN
- Este mensaje configura el tiempo que dura la descarga de la celda durante el balanceo (en minutos).
  - El ID en hexadecimal del mensaje CAN es ID=0x04.
  - DLC = 1 byte (DCTO_MSG_ID)
  - Valor = El número indica un tiempo en minutos:
    - "DCTO0\n" = Se desactiva el TimeOut.
    - "DCTO1\n" = Se produce el balanceo de la Celda Cx durante 0.5 minutos
    - "DCTO2\n" = Se produce el balanceo de la Celda Cx durante 1 minuto
    - "DCTO3\n" = Se produce el balanceo de la Celda Cx durante 2 minutos
    - "DCTO4\n" = Se produce el balanceo de la Celda Cx durante 3 minutos
    - "DCTO5\n" = Se produce el balanceo de la Celda Cx durante 4 minutos
    - "DCTO6\n" = Se produce el balanceo de la Celda Cx durante 5 minutos
    - "DCTO7\n" = Se produce el balanceo de la Celda Cx durante 10 minutos
    - "DCTO8\n" = Se produce el balanceo de la Celda Cx durante 15 minutos
    - "DCTO9\n" = Se produce el balanceo de la Celda Cx durante 20 minutos
    - "DCTO10\n" = Se produce el balanceo de la Celda Cx durante 30 minutos
    - "DCTO11\n" = Se produce el balanceo de la Celda Cx durante 40 minutos
    - "DCTO12\n" = Se produce el balanceo de la Celda Cx durante 60 minutos
    - "DCTO13\n" = Se produce el balanceo de la Celda Cx durante 75 minutos
    - "DCTO14\n" = Se produce el balanceo de la Celda Cx durante 90 minutos
    - "DCTO15\n" = Se produce el balanceo de la Celda Cx durante 120 minutos
  - Se almacena el valor en la dirección de memoria 0x04 de la EEPROM (DCTO_addr)


### Configurar valor N_CELL. Mensaje CAN
- Este mensaje configura el número de celdas conectadas en serie en el BMS.
  - El ID en hexadecimal del mensaje CAN es ID=0x05. (NCELL_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits e indica el número de celdas entre 1 y 12.  
  - Se almacena el valor en la dirección de memoria 0x05 de la EEPROM (NCELL_addr)

### Configurar valor N_CELL_PARALLEL. Mensaje CAN
- Este mensaje configura el número de celdas conectadas en paralelo en el BMS.
  - El ID en hexadecimal del mensaje CAN es ID=0x0F. (NCELL_PARALLEL_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits e indica el número de celdas entre 1 y 99.  
  - Se almacena el valor en la dirección de memoria 0x0A de la EEPROM (NCELL_PARALLEL_addr)


### Configurar valor N_NTC. Mensaje CAN
- Este mensaje configura el número de sensores de temperatura empleados en el BMS.
  - El ID en hexadecimal del mensaje CAN es ID=0x06. (N_NTC_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits e indica el número de termistores entre 1 y 32.
  - Se almacena el valor en la dirección de memoria 0x06 de la EEPROM (N_NTC_addr)


### Configurar tiempo que duerme por ciclo el BMS (T_SLEEP). Mensaje CAN
- Este mensaje configura el tiempo en múltiplos de 100 ms que duerme el BMS tras realizar cada ciclo de operaciones.
  - El ID en hexadecimal del mensaje CAN es ID=0x07. (TSLEEP_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits y se multiplica por 100 ms para tener el tiempo.
  - Se almacena el valor en la dirección de memoria 0x07 de la EEPROM (TSLEEP_addr)


### Forzar el balanceo de las Celdas 1 a la 8 (C0-C7). Mensaje CAN
- Este mensaje activa el balanceo de las celdas cuyo bit en el byte esté a 1, durante el tiempo almacenado en el DCTO.
  - El ID en hexadecimal es ID=0x08 (BAL1TO8_MSG_ID)
  - DLC = 1 byte
  - Valor = Bits a 1 son las celdas a balancear con *LSB=C0* y *MSB=C7*

### Forzar el balanceo de las Celdas 9 a la 12 (C8-C11). Mensaje CAN
- Este mensaje activa el balanceo de las celdas cuyo bit en el byte esté a 1, durante el tiempo almacenado en el DCTO.
  - El ID en hexadecimal es ID=0x09 (BAL9TO12_MSG_ID)
  - DLC = 1 byte
  - Valor = Bits a 1 son las celdas a balancear con *LSB=C8* y *bit3=C11*
  - ID = 0x09  DLC = 1 data[0] = 0b00001001 -> Balancear celdas 9 (C8) y 12 (C11)

### Configurar el tipo de Balanceo a realizar. Mensaje CAN
- Este mensaje configura si el balanceo de las celdas se realiza durante la carga, la descarga, en ambas o ninguna.
Se envía un mnesaje CAN de 1 byte con los siguientes datos:
  - El ID en hexadecimal es ID = 0x0E (TYPE_BALANCING_MSG_ID)
  - DLC = 1 byte
  - Valor = Bit 0 representa el balanceo en la carga. Bit 1 representa el balanceo en la descarga
    - ID = 0x0E DLC = 1 data[0] = 0b00000001 -> Balancear solo en la carga
    - ID = 0x0E DLC = 1 data[0] = 0b00000010 -> Balancear solo en la descarga
    - ID = 0x0E DLC = 1 data[0] = 0b00000011 -> Balancear en carga y descarga
    - ID = 0x0E DLC = 1 data[0] = 0b00000000 -> No balancear en ningún caso
  - Se almacena el valor en la dirección de memoria 0x09 (BALANCING_TYPE_addr)

### Configurar máxima diferencia entre celdas en mV. Mensaje CAN
- Este mensaje CAN configura la máxima diferencia de tensión en mV admisible entre las diferentes celdas.
  - El ID en hexadecimal es ID=0x0D (MAX_DIFF_CELL_MSG_ID)
  - DLC = 1 byte
  - Valor = data[0] * 1mV
  - Se almacena el valor en la dirección de memoria 0x08 de la EEPROM (MAX_DIFF_CELL_addr)

### Configurar offset del sensor de corriente (en mA). Mensaje CAN
- Este mensaje CAN configura el offset a aplicar en la medida del sensor de corriente. Permite un offset entre -32767 y +32767 mA.
  - El ID en hexadecimal es ID=0x10 (CURRENT_OFFSET_MSG_ID)
  - DLC = 2 byte
  - Valor = ((data[0] << 8) | data[1]) - 32767
  - Se almacena el valor en las direcciones de memoria 0x0B y 0x0C de la EEPROM (CURRENT_OFFSET_addr y CURRENT_OFFSET_addr2)

### Pedir las configuraciones de los BMS. Mensaje CAN
- Este mensaje CAN pide todas las configuraciones almacenadas en la EEPROM del BMS. El ID de este mensaje es ID = 0x0B.
El BMS responde con un mensaje con ID = 0x0C y 8 bytes con los datos que ha obtenido de las direcciones de la EEPROM 0x00 a 0x07.
  - El ID en hexadecimal es ID=0x0B (ASK_CONFIG_MSG_ID)
  - DLC = 1 byte
  - Valor = 255

### Enviar todas las configuraciones almacenadas en el BMS. Mensaje CAN
- Este primer mensaje CAN devuelve los diferentes parámetros de configuración en 8 bytes. Su ID es ID = 0x0C.
  - El ID es ID=0x0C (ANSWER_CONFIG_MSG_ID)
  - DLC = 8 bytes
  - Valores:
    - data[0] = VUV
    - data[1] = VOV
    - data[2] = DCTO
    - data[3] = N_CELL
    - data[4] = N_NTC
    - data[5] = MAX_DIFF_CELL_mV
    - data[6] = CELL_BALANCING_C0_C7
    - data[7] = CELL_BALANCING_C8_C11
- Este segundo mensaje CAN devuelve los parámetros de configuración restantes en 5 bytes. Su ID es ID = 0x11.
  - El ID es ID=0x11 (ANSWER_CONFIG_MSG_2_ID)
  - DLC = 5 bytes
  - Valores:
    - data[0] = NCELL_PARALLEL
    - data[1] = CURRENT_OFFSET (MS Byte)
    - data[2] = CURRENT_OFFSET (LS Byte). En su reconstrucción hay que restarle 32767 a la palabra
    - data[3] = T_SLEEP
    - data[4] = BALANCING_TYPE

### Enviar los valores en Voltios de las celdas 1 a la 4. Mensaje CAN
- Este mensaje envía el voltaje de las celdas C1-C4 cada una de ellas como 2 bytes. El valor obtenido debe multiplicarse por 100 uV para tener el Voltaje de la celda.
  - ID=0x40 (DEC=64) (BAT_MSG1_ID)
  - DLC = 8 bytes
  - Valor C1 = (data[0] | (data[1]<<8)) · 100 uV
  - Valor C2 = (data[2] | (data[3]<<8)) · 100 uV   
  - Valor C3 = (data[4] | (data[5]<<8)) · 100 uV
  - Valor C4 = (data[6] | (data[7]<<8)) · 100 uV

### Enviar los valores en Voltios de las celdas 5 a la 8. Mensaje CAN
- Este mensaje envía el voltaje de las celdas C5-C8 cada una de ellas como 2 bytes. El valor obtenido debe multiplicarse por 100 uV para tener el Voltaje de la celda.
    - ID=0x41 (DEC=65) (BAT_MSG2_ID)
    - DLC = 8 bytes
    - Valor C5 = (data[0] | (data[1]<<8)) · 100 uV
    - Valor C6 = (data[2] | (data[3]<<8)) · 100 uV   
    - Valor C7 = (data[4] | (data[5]<<8)) · 100 uV
    - Valor C8 = (data[6] | (data[7]<<8)) · 100 uV

### Enviar los valores en Voltios de las celdas 9 a la 12. Mensaje CAN
- Este mensaje envía el voltaje de las celdas C9-C12 cada una de ellas como 2 bytes. El valor obtenido debe multiplicarse por 100 uV para tener el Voltaje de la celda.
    - ID=0x42 (DEC=66) (BAT_MSG3_ID)
    - DLC = 8 bytes
    - Valor C9 = (data[0] | (data[1]<<8)) · 100 uV
    - Valor C10 = (data[2] | (data[3]<<8)) · 100 uV   
    - Valor C11 = (data[4] | (data[5]<<8)) · 100 uV
    - Valor C12 = (data[6] | (data[7]<<8)) · 100 uV

### Enviar la Temperatura de las NTC 1 a la 8. Mensaje CAN
- Este mensaje envía cada temperatura en 1 byte de datos.
  - ID = 0x43 (DEC=67) (TEMP_MSG1_ID)
  - DLC = 8 bytes
  - NTC0 a NTC7 = data[0] a data[7]. Debe restarse a cada entero 5 y multiplicar por 0,3.

### Enviar la Temperatura de las NTC 9 a la 16. Mensaje CAN
- Este mensaje envía cada temperatura en 1 byte de datos.
  - ID = 0x44 (DEC=68) (TEMP_MSG2_ID)
  - DLC = 8 bytes
  - NTC8 a NTC15 = data[0] a data[7]. Debe restarse a cada entero 5 y multiplicar por 0,3

### Enviar la Temperatura de las NTC 17 a la 24. Mensaje CAN
- Este mensaje envía cada temperatura en 1 byte de datos.
  - ID = 0x45 (DEC=69) (TEMP_MSG3_ID)
  - DLC = 8 bytes
  - NTC16 a NTC23 = data[0] a data[7]. Debe restarse a cada entero 5 y multiplicar por 0,3

### Enviar la Temperatura de las NTC 24 a la 32. Mensaje CAN
- Este mensaje envía cada temperatura en 1 byte de datos.
  - ID = 0x46 (DEC=70) (TEMP_MSG4_ID)
  - DLC = 8 bytes
  - NTC16 a NTC31 = data[0] a data[7]. Debe restarse a cada entero 5 y multiplicar por 0,3

### Enviar el valor del SOC. Mensaje CAN
- Este mensaje envía el SOC del pack como un valor entre 0-10000 (2 byte) correspondiente a 0-100% multiplicado por 100.
  - ID = 0x47 (DEC=71) (SOC_MSG_ID)
  - DLC = 2 byte
  - Mensaje CAN:
    - data[0] = round(SOC * 100) >> 8
    - data[1] = round(SOC *100) & 0b0000000011111111
  - Valor = (uint16_t(data[1] << 8) | data[0])/100.0

### Enviar el valor del SOH. Mensaje CAN
- Este mensaje envía el SOC del pack como un valor entre 0-10000 (2 byte) correspondiente a 0-100% multiplicado por 100.
  - ID = 0x48 (DEC=72) (SOH_MSG_ID)
  - DLC = 2 byte
  - Mensaje CAN:
    - data[0] = round(SOH * 100) >> 8
    - data[1] = round(SOH *100) & 0b0000000011111111
  - Valor = (uint16_t(data[1] << 8) | data[0])/100.0

### Enviar la corriente que circula por la batería. Mensaje CAN
- Este mensaje envía la corriente medida por el BMS (siendo positiva para la carga de la batería) y negativa para la descarga. Se envía el valor como un entero de 4 bytes
con el valor de corriente en mA. Por ejemplo, la lectura de corriente -23.568 A, se convierto al entero (de 32 bits) -23568 y se envían sus 4 bytes.
  - ID = 0x49 (DEC = 73) (CURRENT_MSG_ID)
  - DLC = 4 bytes
  - Valor = (data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) (mA)
