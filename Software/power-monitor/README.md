# power-data

## Descripción
Software desarrollado para monitorizar el consumo de corriente y la tensión de un
sistema usando el integrado INA-219 (https://pypi.org/project/pi-ina219/) y la conexión I2C a una Raspberry PI.
El servicio del sistema se ejecuta cada 5 minutos y almacena en un fichero .json los
datos capturados del día correspondiente, ordenados en directorios.

## Requerimientos
Requiere el módulo de Python INA219.

    $ sudo apt-get install python3-pip
    $ sudo pip3 install pi-ina219

Para la compilación del paquete .deb requiere:
~~~
  - dh-make
  - debmake
  - debhelper
  - devscripts
  - dpkg-dev
  - make
  - python3
  - build-essential
~~~

## Empaquetado

Para hacer el paquete con el servicio del sistema, se debe ejecutar:

    $ debuild -us -uc

Se debe estar en la carpeta 'power_monitor_d.

Mas información [aquí](https://blog.packagecloud.io/eng/2016/12/15/howto-build-debian-package-containing-simple-shell-scripts/ "Documentación dh-make").

## Localización de la instalación

Una vez instalado el .deb se genera un directorio en /usr/share llamado 'power-data'
que contiene 'src', 'data' y 'logs', donde se aloja el script, los datos y los logs.
Los ficheros del servicio de superusuario se alojan en '/lib/systemd/system'

    $ sudo dpkg -i power-monitor-d_1.0_all.deb

## Desinstalación

Para proceder a desinstalar el paquete, debe ejecutarse:

    $ sudo dpkg -r power-monitor-d

Se eliminan todos los ficheros salvo la ruta '/usr/share/power-data/data' que debe eliminarse a mano (para permitir no perder los datos .json).

## Revisión del estado del timer y el servicio

    $ systemctl status power-monitor-d.service
    $ systemctl status power-monitor-d.timer
    $ systemctl start power-monitor-d.service
    $ systemctl stop power-power_monitor_d.service
    $ systemctl daemon-reload

## Licencia
- El paquete: [GPLv3](LICENSE) © 2021 Augusto Samuel Hernández Martín - Universidad de La Laguna
