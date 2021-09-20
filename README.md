# BMS-BatterySystem
Repositorio con el diseño de un BMS y una batería como TFM.

# Sistema diseñado

El diagrama de bloques del sistema de alimentación y la batería es el siguiente:


![Diagrama de bloques del Sistema](https://github.com/AugustoS97/BMS-BatterySystem-TFM/blob/main/img/Diagrama_Bloques_alimentacion.png?raw=true)

La batería diseñada es la siguiente:
![Diagrama de bloques del Sistema](https://github.com/AugustoS97/BMS-BatterySystem-TFM/blob/main/img/render_bateria.png?raw=true)

Los módulos electrónicos diseñados son los siguientes:

## BMS
![Diagrama de bloques del Sistema](https://github.com/AugustoS97/BMS-BatterySystem-TFM/blob/main/img/render_pcb_bms.png?raw=true)

## CAN2Serial
![Diagrama de bloques del Sistema](https://github.com/AugustoS97/BMS-BatterySystem-TFM/blob/main/img/render_pcb_can2serial.png?raw=true)

## CAN2RS485
![Diagrama de bloques del Sistema](https://github.com/AugustoS97/BMS-BatterySystem-TFM/blob/main/img/render_pcb_epsolarcan.png?raw=true)

# Contenido del repositorio

## Software
- BMS CAN Programmer: Puede encontrarse la herramienta BMS_CAN_Programer_GUI que debe ejecutarse en Linux con Qt 5 instalado. Y el firmware preparado para grabar en un ATMega 328P empleando la herramienta PlatformIo que realiza la conversión de los mensajes CAN del BMS desarrollado a mensajes serial que son leídos por la herramienta BMS_CAN_Programmer_GUI.
- SOC_estimation: Contiene los ficheros .csv de diferentes ensayos realizados a las celdas INR21700 de Samsung y en el directorio /src el script a ejecutar por terminal para obtener las curvas del ensayo. La carpeta /figures contiene las gráficas generadas.
- SmartBMS: Contiene el firmware del microcontrolador ATMega 328P del módulo de BMS. Debe flashearse empleando Platformio.
- bms_data_processing: Herramienta para ejecutar desde terminal e indicar la ruta del fichero .csv de datos del BMS generado así como número de celdas en serie y número de termistores.
-  power-monitor: Herramienta para instalar en una Raspberry Pi 3B+ que monitoriza el consumo de la misma a partir de las lecturas del integrado INA219 vía I2C. Almacena los ficheros capturados en un .json dentro del directorio /usr/share/power/data. Se generta un serivicio del sistema que puede ser gestionado mediante systemd.
-  power-data-processing: Herramienta que genera los gráficos de los datos capturados por power-monitor. Accede directamente a la ruta /home/$USER/power-data/data. Los diferentes ficheros python deben ubicarse en /home/$USER/power-data/src.

## Diseno-Electronico
- SmartBMS: Módulo electrónico de un BMS pra la medición de hasta 12 paquetes de celdas en serie, con comunicación CAN y medición de hasta 32 termistores NTC de 10KOhm. Opera con tensiones de alimentación de hasta 75V.
- CAN2Serial: Módulo electrónico para la interfaz de conversión de CAN a Serial para emplear la herramienta BMS_Programer_GUI y configurar el BMS.
- EPSolar-CAN: Módulo electrónico para la conversión de los mensajes CAN del BMS a mensajes para un cargador solar EPEver (mediante RS485).

## Diseno-Mecanico
- Bateria_horizontal.FCStd: Fichero FreeCAD con el diseño de un sistema de batería modular de 14.4V y 84Ah.
- Carcasa_Can2Serial.FCStd: Fichero FreeCAD con el diseno e la carcasa para la placa de circuito impreso de CAN2Serial
- STL: Contiene los modelos 3D de las diferentes piezas.
