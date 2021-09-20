# BMS-data-processing
Script de python 3 para ejecutar desde terminal. Permite generar las gráficas de estado de la batería y el BMS a partir de los datos recogidos por el BMS a través de la herramienta BMS-Programmer-GUI. El código fuente se localiza en '/src', las figuras generadas en 'figures' y los datos en 'data'

# Licencia
- El paquete: [GPLv3](LICENSE) © 2021 Augusto Samuel Hernández Martín - Universidad de La Laguna

# Requerimientos Python
Se precisan las siguientes librerías:
~~~
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
python3 bms-data-processing-d nombre_fichero.csv number_of_cells number_of_temp_sensor
~~~
