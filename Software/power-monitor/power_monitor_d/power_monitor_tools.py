#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Script de medición de consumos de un nodo y almacenamiento de los datos

*** Script para almacenar todas las herramientas ***

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Augusto S. Hernández (ahernama@ull.edu.es) - 2021
'''
import os
from os import path
from datetime import datetime
import time
import json
from pathlib import Path
import subprocess
import requests
import psutil
from ina219 import INA219
from ina219 import DeviceRangeError

SHUNT_OHMS = 0.1
MAX_EXPECTED_AMPS = 3.0

SCRIPT_NAME = "power_monitor_tools.py"

#Para almacenar los datos en /home/$USER/power-data
#ROOT_PATH =  os.path.expanduser("~/power-data")
#LOG_PATH =  os.path.expanduser("~/power-data")
#CONFIG_PATH = os.path.expanduser("~/power-data/conf/")

#Para almacenar los datos en /usr/share/power-data
ROOT_PATH =  "/usr/share/power-data"
LOG_PATH =  "/usr/share/power-data"
CONFIG_PATH = "/usr/share/power-data/conf/"

def get_cpuload():
    cpuload = psutil.cpu_percent(interval=1, percpu=False)
    return cpuload

def get_ramload():
    san = subprocess.check_output(['free','-m']).decode('utf-8')
    lines = san.split('\n')
    ram_used = int(lines[1].split()[2])
    ram_total = int(lines[1].split()[1])
    ram_load = round((ram_used/ram_total)*100.0,1) #Se obtine el % de uso de RAM con 1 decimal
    return ram_load

def check_fs_path(ROOT_PATH,year,month,day):
    '''Función para verificar que la ruta para almacenar el fichero, existe. En caso contrario se creará.'''
    PATH=ROOT_PATH + '/data'
    PATH_YEAR = PATH + "/A"+str(year)
    PATH_MONTH = PATH_YEAR + "/M"+str(month)
    PATH_DAY = PATH_MONTH + "/D"+str(day)

    if not(path.exists(PATH)): # Verificamos que existe el fichero raíz
        write_log(SCRIPT_NAME, "El directorio raíz no existe. Creando el directorio "+PATH+".")
        dir = Path(PATH)
        dir.mkdir(parents=True)
    if not(path.exists(PATH_YEAR)): # Verificamos que existe el fichero del año
        write_log(SCRIPT_NAME, "El directorio correspondiente al año no existe. Creando el directorio "+PATH_YEAR+".")
        dir = Path(PATH_YEAR)
        dir.mkdir(parents=True)
    if not(path.exists(PATH_MONTH)): # Verificamos que existe el fichero del mes
        write_log(SCRIPT_NAME, "El directorio correspondiente al mes no existe. Creando el directorio "+PATH_MONTH+".")
        dir = Path(PATH_MONTH)
        dir.mkdir(parents=True)
    if not(path.exists(PATH_DAY)): # Verificamos que existe el fichero del día
        write_log(SCRIPT_NAME, "El directorio correspondiente al día no existe. Creando el directorio "+PATH_DAY+".")
        dir = Path(PATH_DAY)
        dir.mkdir(parents=True)

def create_log_dir():
    PATH = LOG_PATH+'/logs'

    if not(path.exists(PATH)):
        dir = Path(PATH)
        dir.mkdir(parents=True)

def write_log(script_name, msg):
    '''Función para crear mensajes de log del sistema'''
    now = datetime.now()
    dt_string = now.strftime("[%d/%m/%Y %H:%M:%S] ") # Creamos el mensaje que queremos guardar
    f_msg = dt_string+script_name+": "+msg+"\n"
    print(f_msg.rstrip())

    try:
        with open(LOG_PATH+"/logs/power-data.log", "a") as log_file:
            log_file.write(f_msg)
            log_file.close()
    except FileNotFoundError:
        open(LOG_PATH+"/logs/power-data.log", "w").close()

def list_json_files_full_path(path):
    '''Función para listar los ficheros JSON que contienen datos del sistema'''
    files_path = []
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(".json"):
                files_path.append(os.path.abspath(os.path.join(root, file)))

    files_path.sort()
    return files_path

def data_to_json(measure_power, measure_temp, date_and_time, cpu_load, ram_load, destiny):
    '''Función para convertir los datos de cada lectura recibidos como lista a fichero JSON'''

    new_data = { # Codificación de datos del sensor
                'year': date_and_time[2],
                'month': date_and_time[1],
                'day': date_and_time[0],
                'hour': date_and_time[3],
                'minutes': date_and_time[4],
                'seconds': date_and_time[5],
                'current [mA]': measure_power[0],
                'voltage [V]': measure_power[1],
                'power [mW]': measure_power[2],
                'Temp. CPU [C]': measure_temp[0],
                'CPU Load [%]' : cpu_load,
                'RAM Load [%]' : ram_load}

    # Escritura de los nuevos datos
    if(path.exists(destiny)):
        # Si el fichero existe damos por hecho que contiene datos
        with open(destiny, "r") as jsonFile:
            data = json.load(jsonFile)

        data['data'].append(new_data)

        with open(destiny,"w") as jsonFile:
            json.dump(data, jsonFile)
            write_log(SCRIPT_NAME, "Se han escrito datos en el fichero "+destiny+".")
    else:
        # El fichero no existe por lo tanto hay que crearlo
        data = {}
        data['data'] = []
        data['data'].append(new_data)
        with open(destiny,"w") as jsonFile:
            json.dump(data, jsonFile)
            write_log(SCRIPT_NAME, "Se han escrito datos en el fichero "+destiny+".")

def read_data_from_json(filepath,data_key):
    '''Función para devolver los datos procedentes de un json en una lista'''
    data_extracted = []
    with open(filepath) as json_file:
        data = json.load(json_file)
        for p in data['data']:
            data_extracted.append(p[data_key])

    return data_extracted


def strip_date_from_path(path):
    '''Función para obtener los el año, mes y día desde la ruta del directorio'''
    info = []
    separator = "/"
    str = path.split(separator)
    info.append(separator.join(str[:-5])+"/") #root
    info.append(str[-5][1:]) #year
    info.append(str[-4][1:]) #month
    info.append(str[-3][1:]) #day

    return info

def config_ina():
    ina = INA219(SHUNT_OHMS, MAX_EXPECTED_AMPS)
    ina.configure(ina.RANGE_16V)
    #print("Configurado el INA")

def read_ina():
    '''Funcion para obtener las medidas de potencia en una lista'''
    ina = INA219(SHUNT_OHMS, MAX_EXPECTED_AMPS)
    write_log(SCRIPT_NAME, "Configurando y Despertando INA-219")
    ina.configure(ina.RANGE_16V)
    ina.wake()
    write_log(SCRIPT_NAME, "INA-219 configurado correctamente")
    measure = []
    try:
        #Devuelve una lista con el siguiente orden: Corriente (mA), Voltaje, Potencia (mW)
        measure.append(ina.current())
        measure.append(ina.voltage())
        measure.append(ina.power())
    except DeviceRangeError as e:
        write_log(SCRIPT_NAME, "Current Overflow.")
    ina.sleep() #Duerme
    return measure

def get_temperatures():
    '''Función para obtener las temperaturas del núcleo'''
    #Se almacena temperatura de CPU y GPU en una lista
    temperatures = []
    #Se obtiene la Tª de CPU
    thermal_zone = subprocess.Popen(['cat', '/sys/class/thermal/thermal_zone0/temp'], stdout=subprocess.PIPE)
    out_cpu, err_cpu = thermal_zone.communicate()
    cpu_temp = int(out_cpu.decode())/1000

    #Se obtiene temperatura CPU
    #measure_temp_gpu = subprocess.Popen([', que '], stdout=subprocess.PIPE)
    #out_gpu, err_gpu = measure_temp_gpu.communicate()
    #gpu_temp = out_gpu.decode().split('=')[1].split('\'')[0]
    #Se crea la lista con las temperaturas
    temperatures.append(cpu_temp)
    #temperatures.append(gpu_temp)

    write_log(SCRIPT_NAME, "Tª CPU y GPU medidas correctamente.")
    return temperatures

def get_date_as_list():
    '''Función para obtener el dia y la hora como una lista'''
    now = datetime.now()
    date_and_time = []
    date_and_time.append(now.strftime("%d"))
    date_and_time.append(now.strftime("%m"))
    date_and_time.append(now.strftime("%Y"))
    date_and_time.append(now.strftime("%H"))
    date_and_time.append(now.strftime("%M"))
    date_and_time.append(now.strftime("%s"))
    return date_and_time

def date_as_string():
    '''Función para obtener el dia mes y año como string'''
    now = datetime.now()
    dt_string = now.strftime("%d%m%Y")
    return dt_string
