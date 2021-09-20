# BMS-data-processing

Script de python 3 para ejecutar desde terminal. Permite generar las gráficas de los datos recogidos por el servicio del sistema 'power-monitor'. La carpeta 'power-data' debe localizar en '/home/$USER/power-data'. El código fuente se localiza en '/src', los logs de procesamiento en '/logs' y los datos en '/data'.

# Estructra de ficheros
El conjunto de ficheros deben de localizarse de la siguiente forma:


~~~
|--- power-data
|--- data
|   |--- Year
|   |   |--- Month
|   |   |   |--- Day
|   |   |   |   |--- power-dataYYMMDDDD.json
|--- src
|   |--- power_data_plot_tools
|   |--- power_data_tools
|   |-- power-data-processing-d
|--- logs
|   |--- _processed_plot.log
|   |--- power-data.log
~~~

# Requerimientos Python
Se precisan las siguientes librerías:
~~~
- matplotlib
- os
- datetime
- time
- json
- pathlib
- subprocess
- requests
- sys
- numpy
- csv
~~~

# Ejecucion del script

Se debe ejecutar desde terminal como se indica:
~~~
python3 power-data-processing-d
~~~
