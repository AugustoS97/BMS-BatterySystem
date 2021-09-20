# BMS-BatterySystem
Repositorio con el diseño de un BMS y una batería como TFM.

# Sistema diseñado

![Diagrama de bloques del Sistema]
(https://github.com/AugustoS97/BMS-BatterySystem-TFM/blob/main/img/Diagrama_Bloques_alimentacion.png?raw=true)


# Contenido del repositorio
- BMS CAN Programmer: Puede encontrarse la herramienta BMS_CAN_Programer_GUI que debe ejecutarse en Linux con Qt 5 instalado. Y el firmware preparado para grabar en un ATMega 328P empleando la herramienta PlatformIo que realiza la conversión de los mensajes CAN del BMS desarrollado a mensajes serial que son leídos por la herramienta BMS_CAN_Programmer_GUI.
- SOC_estimation: Contiene los ficheros .csv de diferentes ensayos realizados a las celdas INR21700 de Samsung y en el directorio /src el script a ejecutar por terminal para obtener las curvas del ensayo. La carpeta /figures contiene las gráficas generadas.
- SmartBMS: Contiene el firmware del microcontrolador ATMega 328P del módulo de BMS. Debe flashearse empleando Platformio.
- bms_data_processing: Herramienta para ejecutar desde terminal e indicar la ruta del fichero .csv de datos del BMS generado así como número de celdas en serie y número de termistores.
-  power-monitor: Herramienta para instalar en una Raspberry Pi 3B+ que monitoriza el consumo de la misma a partir de las lecturas del integrado INA219 vía I2C. Almacena los ficheros capturados en un .json dentro del directorio /usr/share/power/data. Se generta un serivicio del sistema que puede ser gestionado mediante systemd.
-  power-data-processing: Herramienta que genera los gráficos de los datos capturados por power-monitor. Accede directamente a la ruta /home/$USER/power-data/data. Los diferentes ficheros python deben ubicarse en /home/$USER/power-data/src.
-  Diseño Electrónico: Se presenta 3 subcarpetas, cada una de ellas con los ficheros de KiCAD de los diferentes módulos (BMS, CAN2Serial, RS4852CAN).
-  Diseño Mecánico: Se presenta un fihcero para FreeCAD 0.19 con un diseño de soporte de celdas 21700 para la creación de una batería modular.
