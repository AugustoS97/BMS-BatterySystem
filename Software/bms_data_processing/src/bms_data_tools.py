#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Herramientas de lectura y escritura de ficheros y logs para procesamiento datos
del BMS

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


SCRIPT_NAME = "bms_data_tools.py"


def write_log(script_name, msg):
    '''Función para crear mensajes de log del sistema'''
    now = datetime.now()
    dt_string = now.strftime("[%d/%m/%Y %H:%M:%S] ") # Creamos el mensaje que queremos guardar
    f_msg = dt_string+script_name+": "+msg+"\n"
    print(f_msg.rstrip())
