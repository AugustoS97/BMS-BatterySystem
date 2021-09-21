# BMS-BatterySystem
Repositorio con el diseño de un BMS y una batería como Trabajo de Fin de Máster.
- Autor: Augusto Samuel Hernández Martín
- Tutor: Fernando Luis Rosa González
- Co-Tutor: Iván Rodríguez Méndez
- Titulación: Máster en Ingeniería Industrial
- Universidad de La Laguna, 2021

# Licencia
- El paquete: [GPLv3](LICENSE) © 2021 Augusto Samuel Hernández Martín - Universidad de La Laguna

# Resumen 
El presente Trabajo Fin de Máster consiste en el diseñado y fabricación de un Sistema de Gestión de Batería (BMS), así como una batería modular configurable a diferentes tensiones y capacidades, con el objetivo de poder alimentar sistemas electrónicos aislados como los dispositivos de capturas de índices de significación eco-acústicos. Se ha llevado a cabo el diseño electrónico de los diferentes módulos que forman el sistema de alimentación y gestión así como el cálculo y diseño de la batería a partir de celdas de ion-litio y su estructura mecánica. Para poder alimentar dicho sistema se ha dimensionado una posible instalación generadora, empleando energía fotovoltaica. Finalmente, se ha realizado la fabricación de los distintos elementos, tanto mecánicos como electrónicos, y el desarrollo software de las diferentes herramientas necesarias para la configuración del BMS, la captura y el análisis de los datos generados. Por último, se han propuesto y analizado diferentes aplicaciones, además de la mencionada como objetivo principal del proyecto, para este sistema de BMS y acumulador.

# Abstract

In the present Master’s Degree Final Project, a Battery Management System (BMS) has been designed and manufactured, as well as a modular battery configurable at different voltages and capacities, with the aim of being able to power isolated electronic systems such as devices for capturing eco-acoustic significance indexes. The electronic design of the different modules that make up the power supply and management system has been carried out, as well as the calculation and design of the battery based on lithium-ion cells and its mechanical structure. In order to be able to supply this system, a possible generating installation has been dimensioned, using photovoltaic energy. Finally, the manufacture of the different elements, both mechanical and electronic, and the software development of the different tools necessary for the configuration of the BMS and the capture and analysis of the data generated have been carried out. Finally, different applications have been proposed and analysed, in addition to the one mentioned as the main objective of the project, for this BMS and accumulator system.

# Imágenes del sistema desarrollado
![BMS](https://github.com/AugustoS97/BMS-BatterySystem/blob/main/img/montaje_bateria.jpg?raw=true)

![Bateria](https://github.com/AugustoS97/BMS-BatterySystem/blob/main/img/montaje_bateria_2.jpg?raw=true)

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
