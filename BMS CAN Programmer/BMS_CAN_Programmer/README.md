# BMS_CAN_programer
Firmware del nodo que recibe los datos del CANBUS y comunica con el PC por Serial y configura el BMS por CAN envíando los parámetros transmitidos desde el PC por Serial.
# Mensajes a Enviar por puerto Serial
Para realizar la comunicación con el CAN debe Enviarse un mensaje por serial, configurado a 115200 baudios, formado por:

- 1 caracter ASCII: Es el identificador del mensaje.
- 1 a 3 caracteres numéricos ASCII: Correpsondiente al valor de configuración.
- Salto de línea: Indica el final del mensaje.

### Configurar en el BMS el valor de tensión mínima de celda. VUV

- Se envía un mensaje con ID='A' seguido de un número entre 0 y 255. El valor en voltios corresponde al producto de ese numero por 0,02. Por ejemplo:

  - "A125\n" = 125 * 0,02 = 2,5 V

### Configurar en el BMS el valor de tensión máxima de celda. VOV

  - Se envía un mensaje con ID='B' seguido de un número entre 0 y 255. El valor en voltios corresponde al producto de ese numero por 0,02. Por ejemplo:

    - "B125\n" = 125 * 0,02 = 2,5 V

### Valor de DCTO
- El DCTO es el tiempo que dura activado el balanceo de una celda. Debe Enviarse el ID='C' seguido de uno de los siguientes números:

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

### Número de celdas conectadas
- Configura el número de celdas en serie conectadas al BMS, comenzando por la celda C0. Se debe Enviar el ID='D' seguido de un numero entre 1 y 12 para indicar el número de celdas:
  - "D12\n" = Configurar 12 celdas en serie

### Número de Termistores (NTC) conectados
- Configura el número de NTC conectadas al BMS, comenzando por la NTC0. Se debe Enviar el ID='E' seguido de un numero entre 1 y 32 para indicar el número de sensores:
  - "E12\n" = Configurar 12 sensores NTC

### Máxima diferencia de voltaje entre celdas
- Configura la diferencia de tensión máxima admisible entre celdas. Este parámetro es utilizado por el algoritmo de balanceo para activar el balanceo de las celdas cuando se supera este umbral. Debe Enviarse el ID='I' seguido de un número entre 0 y 255 que indica los mV máximos de diferencia:
  - "I220\n" = Máxima diferencia admisible entre celdas de 220 mV

### Forzar el balanceo de las celdas 1 a la 8 (C0-C7)
- Activa el balanceo de las celdas cuyas posiciones en el byte estén a 1 (siendo el *LSB= C0* y *MSB=C7*), durante el tiempo indicado mediante el mensaje de *configurar DCTO*. Debe Enviarse el ID='G' seguido de 1 byte indicando las celdas a balancear:
  - "G57\n" = Balancear las celdas 1,4,5 y 6 (0b00111001)

### Forzar el balanceo de las celdas 9 a la 12 (C8-C11)
- Activa el balanceo de las celdas cuyas posiciones en el byte estén a 1 (siendo el *LSB= C8* y *bit4=C11*), durante el tiempo indicado mediante el mensaje de *configurar DCTO*. Debe Enviarse el ID='H' seguido de 1 byte indicando las celdas a balancear:
  - "H9\n" = Balancear las celdas 9 y 12 (C8 y 11) )(0b00001001).

### Activar o desactivar el balanceo automático
- Activa el balanceo de las celdas de forma automática durante la carga de la batería, la descarga o ambas, durante el tiempo indicado mediante el mensaje de *configurar DCTO*. Debe Enviarse el ID='J' seguido de 2 bits que indican el tipo de balanceo:
  - "J0\n" = Balanceo desactivado (0b00)
  - "J1\n" = Balanceo exclusivamente durante la carga (0b01)
  - "J2\n" = Balanceo exclusivamente durante la descarga (0b10)
  - "J3\n  = Balanceo durante la carga y la descarga(0b11)

### Numero de celdas conectadas en paralelo
- Configura el número de celdas conectadas en paralelo en el BMS. Debe envisarse el ID='K' seguido del valor (número de celdas):
  - "K10\n" = 10 celdas conectadas en paralelo

### Offset del sensor de Corriente
- Configura el offset a aplicar en la medida de corriente. Debe enviarse el ID='L' seguido del valor (positivo o negativo) entre -220 y 220 mA:
  - "L-110\n" = Offset de -110 mA en la medida de corriente

### Tiempo que duerme por ciclo
- Configura el tiempo de reposo en cada ciclo del BMS. Debe enviarse el ID='F' seguido de un valor entre 0 y 255 que es el tiempo en ms que duerme el BMS.
  - "F10\n" = Tiempo de reposo de 10 ms

### Pedir los valores de configuracion de la EEPROM del BMS.
- Pide todos los parámetros de configuración que presenta almacenados el BMS en su EEPROM. Debe Enviarse el ID='Z' seguido de un 1:
  - "Z1\n" = Pedir todos los parámetros de configuración.
- Se espera recibir dos mensaje, uno de 8 bytes y otro de 5 por CAN, con 1 byte por valor de configuración en el siguiente orden (desde el LSB al MSB): VUV, VOV, DCTO, NCELL, NNTC, DIF_CELL_V, CELL_BALANCING_C0-C7, CELL_BALANCING_C8-C11 para el primer mensaje. El segundo mensaje presenta (desde LSB a MSB): NCELL_PARALLEL, CURRENT_OFFSET, CURRENT_OFFSET_SIGN, T_SLEEP, TYPE_BALANCING



# Mensajes a recibir por puerto Serial
Por Puerto Serie se reciben en el PC los valores configurados con los comandos anteriores, una vez son pedidos al BMS, así como los parámetros medidos por el mismo. Los indicadores de los mensajes recibidos se corresponden con los indicadores de los mensajes envíados por serial.

### Obtener del BMS el valor de tensión mínima de celda. VUV

- Se recibe un mensaje con ID='A' seguido de un número entre 0 y 255. El valor en voltios corresponde al producto de ese numero por 0,02. Por ejemplo:

  - "A125\n" = 125 * 0,02 = 2,5 V

### Obtener del BMS el valor de tensión máxima de celda. VOV

  - Se recibe un mensaje con ID='B' seguido de un número entre 0 y 255. El valor en voltios corresponde al producto de ese numero por 0,02. Por ejemplo:

    - "B125\n" = 125 * 0,02 = 2,5 V

### Obtener el valor de DCTO
- El DCTO es el tiempo que dura activado el balanceo de una celda. Cuando se ha solicitado su valor, se recibe el ID='C' seguido de uno de los siguientes números:

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

### Obtener número de celdas conectadas
- Se obtiene el número de celdas conectadas comenzando por la celda C0. Se recibe el ID='D' seguido de un numero entre 1 y 12 para indicar el número de celdas:
  - "D12\n" = Configurar 12 celdas en serie

### Obtener número de Termistores (NTC) conectados
- Se obtiene el número de NTC conectadas al BMS, comenzando por la NTC0. Se recibe el ID='E' seguido de un numero entre 1 y 32 para indicar el número de sensores:
  - "E12\n" = Configurar 12 sensores NTC

### Tiempo que duerme por ciclo
- Se obtiene el tiempo de reposo en cada ciclo del BMS. Se recibe el ID='F' seguido de un valor entre 0 y 255 que es el tiempo en ms que duerme el BMS.
  - "F10\n" = Tiempo de reposo de 10 ms

### Obtener celdas entre la celda 1 y la celda 8 (C0-C7) balanceandose
- Se obtiene las celdas que se están balanceando en un numero decimal cuyas posiciones en el byte estén a 1 para las celdas que se están balanceando (siendo el *LSB= C0* y *MSB=C7*). Se recibe el ID='G' seguido de 1 byte indicando las celdas que se están balanceando:
  - "G57\n" = Las celdas 1,4,5 y 6 (0b00111001) se están balanceando.

### Obtener celdas entre la celda 9 y la celda 12 (C8-C11) balanceandose
- Se obtiene las celdas que se están balanceando como aquellas cuyas posiciones en el byte estén a 1 (siendo el *LSB= C8* y *bit4=C11*). Se recibe el ID='H' seguido de 1 byte decimal indicando las celdas que se están balanceando:
  - "H9\n" =Las celdas 9 y 12 (0b00001001) (C8 y 11) están balanceandose.

### Obtener la máxima diferencia de voltaje entre celdas
- Se obtiene la diferencia de tensión máxima admisible entre celdas. Se recibe el ID='I' seguido de un número entre 0 y 255 que indica los mV máximos de diferencia almacenados en el BMS:
  - "I220\n" = Máxima diferencia admisible entre celdas de 220 mV

### Activar o desactivar el balanceo automático
- Se obtiene el tipo de balanceo que se realiza a las celdas. Se recibe el ID='J' seguido de 2 bits que indican el tipo de balanceo:
  - "J0\n" = Balanceo desactivado
  - "J1\n" = Balanceo exclusivamente durante la carga
  - "J2\n" = Balanceo exclusivamente durante la descarga
  - "J3\n  = Balanceo durante la carga y la descarga

### Obtener numero de celdas conectadas en paralelo
- Se obtiene el número de celdas conectadas en paralelo en el BMS. Se recibe el ID='K' seguido del valor (número de celdas):
  - "K10\n" = 10 celdas conectadas en paralelo

### Obtener Offset del sensor de Corriente
- Se obtiene el offset a aplicar en la medida de corriente. Se recibe el ID='L' seguido del valor (positivo o negativo) entre -220 y 220 mA:
  - "L-110\n" = Offset de -110 mA en la medida de corriente

### Voltaje de las celdas 1 a la 4 (C0-C3)
- Se obtiene la tensión de las celdas C0-C3 mediante mensajes seriales con ID='M' seguido del número de la celda y un valor entre 0 y 65536 que se debe multiplicar por 100 uV para obtener la tensión de la celda.
  - "M163000\n" = El voltaje de la celda 1 es 63000 * 100 * 10^(-6) = 6,3 V

### Voltaje de las celdas 5 a la 8 (C4-C7)
- Se obtiene la tensión de las celdas C4-C7 mediante mensajes seriales con ID='N' seguido del número de la celda y un valor entre 0 y 65536 que se debe multiplicar por 100 uV para obtener la tensión de la celda.
  - "N163000\n" = El voltaje de la celda 1 es 63000 * 100 * 10^(-6) = 6,3 V

### Voltaje de las celdas 9 a la 12 (C8-C11)
- Se obtiene la tensión de las celdas C8-C11 mediante mensajes seriales con ID='O' seguido del número de la celda y un valor entre 0 y 65536 que se debe multiplicar por 100 uV para obtener la tensión de la celda.
  - "O163000\n" = El voltaje de la celda 1 es 63000 * 100 * 10^(-6) = 6,3 V

### Temperatura medida por la NTCx
- Se obtiene la temperatura de la NTCx como un byte que debe convertirse a un entero, restarle 5 y multiplicarse por 0.3 para obtener la Temperatura en ºC. Se recibe el ID='P' seguido de un número entre 01 y 32 (2 bytes de caracteres ASCII) y posteriormente un valor entre 0 y 255 correspondiente a la temperatura:
  - "P08225\n" = Temperatura de la celda 8 es de (225 - 5)*0.3 = 66 ºC

### Valor de la corriente medida en la batería
- Se obtiene el valor de la corriente a partir de la reconstrucción del entero partiendo de 4 bytes de datos. Se recibe el valor de la corriente en mA y separado en 4 bytes, que se reconstruyen como se muestra a continuación:
  - current = ((can_msg_in.data[0]) | (can_msg_in.data[1] << 8) | (can_msg_in.data[2] << 16) | (can_msg_in.data[3] << 24))
- El ID que se envía por puerto serie es ID='Q' seguido del valor de corriente (mA) en decimal.
  - "Q-16452\n" = Corriente de -16.542 Amperios (descarga)

### Valor de SOC del Pack
- Se obtiene el valor del estado de carga como dos byte que representa el porcentaje entre 0 y 100 multiplicado por 100. Se recibe por serial en el PC el ID='T' seguido del porcentaje multiplicado por 100. Ese valor debe dividirse entre 100:
 - "T9980\n" = el SOC es de 99,80 %

### Valor de SOH del Pack
- Se obtiene el valor del estado de salud como dos byte que representa el porcentaje entre 0 y 100. Se recibe por serial en el PC el ID='U' seguido de un valor entre 0 y 100 multiplicado por 100. Ese valor debe dividirse entre 100:
  - "U9980\n" = el SOH es de 99.80 %


# Mensajes CAN de Configuracion
A continuación se indican los mensajes CAN que son intercambiados entre el nodo de configuracion/monitoreo y el nodo BMS.

### Warning. Mensaje CAN
- Este mensaje se envía cuando hay una sobretemperatura o una tensión demasiado baja o alta. Su ID = 0x00 y está formado por 1 byte. Los bits a 1 indican las alertas que están activas:
  - ID = 0x00 (WARNING_ID)
  - DLC = 1 byte
  - data[0] = x;x;x;x;x;OverTemp;NotCharged;UnderVoltage

### Configurar valor VUV. Mensaje CAN
- Este mensaje configura el valor de tensión mínima de la celda.
  - El ID en hexadecimal del mensaje CAN es ID=0x02. (VUV_MSG_ID)
  - DLC = 1 byte
  - Valor (Voltios) = data[0] * 0,02 V

### Configurar valor VOV. Mensaje CAN
- Este mensaje configura el valor de tensión máxima de la celda.
  - El ID en hexadecimal del mensaje CAN es ID=0x03. (VOV_MSG_ID)
  - DLC = 1 byte
  - Valor (Voltios) = data[0] * 0,02 V

### Configurar valor DCTO. Mensaje CAN
- Este mensaje configura el tiempo que dura la descarga de la celda durante el balanceo.
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

### Configurar valor N_CELL. Mensaje CAN
- Este mensaje configura el número de celdas conectadas en serie.
  - El ID en hexadecimal del mensaje CAN es ID=0x05. (NCELL_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits e indica el número de celdas entre 1 y 12.  

### Configurar valor N_NTC. Mensaje CAN
- Este mensaje configura el número de sensores de temperatura empleados.
  - El ID en hexadecimal del mensaje CAN es ID=0x06. (N_NTC_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits e indica el número de termistores entre 1 y 32.  

### Configurar valor N_CELL_PARALLEL. Mensaje CAN
- Este mensaje configura el número de celdas conectadas en paralelo en el BMS.
  - El ID en hexadecimal del mensaje CAN es ID=0x0F. (NCELL_PARALLEL_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits e indica el número de celdas entre 1 y 99.  
  - Se almacena el valor en la dirección de memoria 0x0A de la EEPROM (NCELL_PARALLEL_addr)

### Configurar tiempo que duerme por ciclo el BMS (T_SLEEP). Mensaje CAN
- Este mensaje configura el tiempo en múltiplos de 15 ms que duerme el BMS tras realizar cada ciclo de operaciones.
  - El ID en hexadecimal del mensaje CAN es ID=0x07. (TSLEEP_MSG_ID)
  - DLC = 1 byte
  - Valor = El byte se convierte a un entero de 8 bits y se multiplica por 100 ms para tener el tiempo.

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

### Configurar máxima diferencia entre celdas en mV. Mensaje CAN
- Este mensaje CAN configura la máxima diferencia de tensión en mV admisible entre las diferentes celdas.
  - El ID en hexadecimal es ID=0x0D (MAX_DIFF_CELL_MSG_ID)
  - DLC = 1 byte
  - Valor = data[0] * 1mV

### Configurar offset del sensor de corriente (en mA). Mensaje CAN
- Este mensaje CAN configura el offset a aplicar en la medida del sensor de corriente. Permite un offset entre -32767 y +32767 mA.
  - El ID en hexadecimal es ID=0x10 (CURRENT_OFFSET_MSG_ID)
  - DLC = 2 byte
  - Valor = ((data[0] << 8) | data[1]) - 32767
  - Se almacena el valor en las direcciones de memoria 0x0B y 0x0C de la EEPROM (CURRENT_OFFSET_addr y CURRENT_OFFSET_addr2)

### Pedir las configuraciones de los BMS. Mensaje CAN
- Este mensaje CAN pide todas las configuraciones almacenadas en la EEPROM del BMS. El ID de este mensaje es ID = 0x0B. El BMS responde con un mensaje con ID = 0x0C y 8 bytes con los datos.
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
- Este segundo mensaje CAN devuelve los parámetros de configuración restantes en 4 bytes. Su ID es ID = 0x11.
  - El ID es ID=0x11 (ANSWER_CONFIG_MSG_2_ID)
  - DLC = 4 bytes
  - Valores:
    - data[0] = NCELL_PARALLEL
    - data[1] = CURRENT_OFFSET (MS Byte)
    - data[2] = CURRENT_OFFSET (LS Byte). En su reconstrucción hay que restarle 32767 a la palabra
    - data[3] = T_SLEEP
    - data[4] = BALANCING_TYPE


### Enviar los valores en Voltios de las celdas 1 a la 4. Mensaje CAN
- Este mensaje envía el voltaje de las celdas C0-C3 cada una de ellas como 2 bytes. El valor obtenido debe multiplicarse por 100 uV para tener el Voltaje de la celda.
  - ID=0x40 (DEC=64) (BAT_MSG1_ID)
  - DLC = 8 bytes
  - Valor C0 = (data[0] | (data[1]<<8)) · 100 uV
  - Valor C1 = (data[2] | (data[3]<<8)) · 100 uV   
  - Valor C0 = (data[4] | (data[5]<<8)) · 100 uV
  - Valor C0 = (data[6] | (data[7]<<8)) · 100 uV

### Enviar los valores en Voltios de las celdas 5 a la 8. Mensaje CAN
- Este mensaje envía el voltaje de las celdas C4-C7 cada una de ellas como 2 bytes. El valor obtenido debe multiplicarse por 100 uV para tener el Voltaje de la celda.
    - ID=0x41 (DEC=65) (BAT_MSG2_ID)
    - DLC = 8 bytes
    - Valor C4 = (data[0] | (data[1]<<8)) · 100 uV
    - Valor C5 = (data[2] | (data[3]<<8)) · 100 uV   
    - Valor C6 = (data[4] | (data[5]<<8)) · 100 uV
    - Valor C7 = (data[6] | (data[7]<<8)) · 100 uV

### Enviar los valores en Voltios de las celdas 9 a la 12. Mensaje CAN
- Este mensaje envía el voltaje de las celdas C8-C11 cada una de ellas como 2 bytes. El valor obtenido debe multiplicarse por 100 uV para tener el Voltaje de la celda.
    - ID=0x42 (DEC=66) (BAT_MSG3_ID)
    - DLC = 8 bytes
    - Valor C8 = (data[0] | (data[1]<<8)) · 100 uV
    - Valor C9 = (data[2] | (data[3]<<8)) · 100 uV   
    - Valor C10 = (data[4] | (data[5]<<8)) · 100 uV
    - Valor C11 = (data[6] | (data[7]<<8)) · 100 uV

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
