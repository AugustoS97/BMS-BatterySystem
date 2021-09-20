#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Herramientas de lectura y escritura de ficheros y logs para procesamiento datos
de potencia de un nodo

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

Iván Rodríguez (irodrigu@ull.es) - 2021
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


SCRIPT_NAME = "power_data_tools.py"

#Para almacenar los datos en /home/$USER/power-data
ROOT_PATH =  os.path.expanduser("~/power-data")
LOG_PATH =  os.path.expanduser("~/power-data")
SORTDATA_PATH =  os.path.expanduser("~/power-data/data")


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

def list_files_full_path(path):
    write_log(SCRIPT_NAME, "Listando ficheros de la ruta "+path+".")
    l = glob.glob(path)
    l.sort()
    return l

def remove_directory(path):
    '''Función para eliminar un directorio del sistema de ficheros'''
    write_log(SCRIPT_NAME, "Eliminando el directorio "+path+".")
    try:
        shutil.rmtree(path)
    except OSError as e:
        print("Error: %s : %s" % (dir_path, e.strerror))

    return 0

def create_processed_file():
    '''Función para crear el fichero para la escritura de los logs'''
    FILE = LOG_PATH+'/logs/'+'_processed.log'
    if not os.path.exists(FILE):
        write_log(SCRIPT_NAME, "No existe el fichero "+FILE+".")
        os.mknod(FILE)
    else:
        write_log(SCRIPT_NAME, "Ya existe el fichero "+FILE+".")

def create_processed_plot_file():
    '''Función para crear el fichero para la escritura de los logs sobre los plots'''
    FILE = LOG_PATH+'/logs/processed_plot.log'
    if not os.path.exists(FILE):
        write_log(SCRIPT_NAME, "No existe el fichero "+FILE+".")
        os.mknod(FILE)
    else:
        write_log(SCRIPT_NAME, "Ya existe el fichero "+FILE+".")

def check_if_plot_processed(name_to_search):
    '''Función para comprobar si uno de los gráficos ha sido procesado'''
    try:
        with open(LOG_PATH+"/logs/"+"_processed_plot.log", "r") as file:
            for line in file:
                if name_to_search in line:
                    return True # Devolvemos un True si encontramos el fichero como procesado
    except FileNotFoundError:
        open(LOG_PATH+"/logs/"+"_processed_plot.log", "w").close()

    return False


def write_processed_log(filename):
    try:
        with open(LOG_PATH+"/logs/"+"_processed.log", "a") as log_file:
            log_file.write(filename+"\n")
    except FileNotFoundError:
        open(LOG_PATH+"/logs/"+"_processed.log", "w").close()

def write_excluded_log(filename):
    try:
        with open(LOG_PATH+'/logs/'+'_excluded.txt', "a") as log_file:
            log_file.write(filename+"\n")
    except FileNotFoundError:
        open(LOG_PATH+'/logs/'+'_excluded.txt', "w").close()

def write_processed_plot_log(filename):
    try:
        with open(LOG_PATH+"/logs/"+"_processed_plot.log", "a") as log_file:
            log_file.write(filename+"\n")
    except FileNotFoundError:
        open(LOG_PATH+"/logs/"+"_processed_plot.log", "w").close()

def check_if_file_processed(name_to_search):
    '''Función para comprobar si uno de los ficheros ha sido procesado'''
    try:
        with open(LOG_PATH+"/logs/"+"_processed.log", "r") as file:
            for line in file:
                if name_to_search in line:
                    return True # Devolvemos un True si encontramos el fichero como procesado
    except FileNotFoundError:
        open(LOG_PATH+"/logs/processed.log", "w").close()

    return False

def list_json_files_full_path(path):
    '''Función para listar los ficheros JSON que contienen datos del sistema'''
    files_path = []
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(".json"):
                files_path.append(os.path.abspath(os.path.join(root, file)))

    files_path.sort()
    return files_path

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

def plot_update(path):
    ''' Función para marcar un gráfico como que debe ser actualizado'''
    try:
        filename = LOG_PATH+"/logs/"+"_processed_plot.log"
        plots = subprocess.check_output("grep "+" "+filename,shell=True)
        plot_string = plots.decode("utf-8").split("\n")[:-1] # Devolvemos la lista de esta forma para que no se cuele un elemento vacío

        # Borramos las correspondientes líneas del fichero

        f = open(filename,'r')
        lst = []
        for line in f:
            for word in plot_string:
                if word in line:
                    write_log(SCRIPT_NAME, "Gráfico "+word+" marcado para actualizar.")
                    line = line.replace(word+"\n",'')
            lst.append(line)
        f.close()

        f = open(filename,'w')
        for line in lst:
            f.write(line)
        f.close()
        write_log(SCRIPT_NAME, "Los gráficos han sido marcados para actualizar.")

    except subprocess.CalledProcessError:
        write_log(SCRIPT_NAME, "El gráfico no se encuentra procesado.")
        return -1
