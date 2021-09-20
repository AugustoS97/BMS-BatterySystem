# BMS-data-processing

Script de python 3 para ejecutar desde terminal. Permite generar las gráficas de estimación del SOC de la celda a partir de un .csv generado por la carga electrónica Rigol DL3010 (https://www.venotek.no/display.aspx?menuid=40017&prodid=5545). Se indica un grado para la estimación polinómica y el valor de resistencia interna de la celda y se obtiene por salida de consola la expresión polinómica que se ajusta a la curva para ese grado. El código fuente se localiza en '/src', las figuras generadas en 'figures' y los datos en 'data'.

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
python3 estimate_SOC.py nombre_fichero.csv grado_regresion_polinomica internal_resistor
~~~
