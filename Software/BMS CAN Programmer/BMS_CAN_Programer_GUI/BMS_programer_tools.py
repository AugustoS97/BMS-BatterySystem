#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Software con Interfaz Gráfica para configurar y leer los datos del BMS por CAN

*** Script para cargar las configuraciones desde el json***

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
Augusto Samuel Hernández Martín (ahernama@ull.es) - 2021
'''

SCRIPT_NAME = "BMS_programer_tools.py"

import json, os
from datetime import datetime

filename_in='config.json'

#id_serial = {
#    'VUV_ID' : 'A',
#    'VOV_ID' : 'B',
#    'DCTO_ID' : 'C',
#    'MAX_DIFF_V_ID' : 'I',
#    'CELL_N_ID' : 'D',
#    'CELL_TEMP_N_ID' : 'E',
#    'BAL_MODE_ID' : 'J',
#    'T_SLEEP_ID' : '',
#    'FORCE_BAL_1_8' : 'G',
#    'FORCE_BAL_9_12' : 'H',
#    'VCELL_GROUP_A' : 'M',
#    'VCELL_GROUP_B' : 'N',
#    'VCELL_GROUP_C' : 'O',
#    'TEMP_VALUE_ID' : 'P',
#    'CURRENT_VALUE_ID' : 'Q',
#    'SOC_VALUE_ID' : 'T'
#}

'''Función para crear mensajes de log del sistema'''
def print_log(msg):
    now = datetime.now()
    dt_string = now.strftime("[%d/%m/%Y %H:%M:%S] ") #Se crea el mensaje a mostrar
    f_msg = dt_string+": "+msg+"\n"
    print(f_msg.rstrip())

''' Se crea el diccionario a partir del json de configuraciones '''
with open(filename_in, "r", encoding="utf8") as json_file:
    id_serial = json.load(json_file)
