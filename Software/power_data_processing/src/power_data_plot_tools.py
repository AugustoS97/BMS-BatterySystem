#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Representación de los datos de potencia de un nodo

*** Script para gestionar las herramientas para la realización de gráficos ***

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

import matplotlib
import matplotlib.cm as cm
from matplotlib import pyplot as plt
import numpy as np
import power_data_tools as tools
from os import path

SCRIPT_NAME = "power_data_plot_tools.py"

def plot_rpi_temp_day(plot_file_path, data_file_path):
    '''Función para generar un gráfico con las temperaturas del núcleo de la Raspberry Pi'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))
    # Extraemos los datos necesarios
    temp = tools.read_data_from_json(data_file_path,"Temp. CPU [C]")
    year = tools.read_data_from_json(data_file_path,"year")
    month = tools.read_data_from_json(data_file_path,"month")
    day = tools.read_data_from_json(data_file_path,"day")
    hour = tools.read_data_from_json(data_file_path,"hour")
    minutes = tools.read_data_from_json(data_file_path,"minutes")

    time = []
    temp = [float(i) for i in temp]
    year = [float(i) for i in year]
    month = [float(i) for i in month]
    day = [float(i) for i in day]
    hour = [float(i) for i in hour]
    minutes = [float(i) for i in minutes]
    # Generamos la lista con las horas disponibles
    for i in range(len(temp)):
        time.append("%2dh:%2dm"%(hour[i],minutes[i]))


    plt.plot(time,temp,'b-',label = "Temperature")
    plt.hlines(sum(temp)/len(temp),time[0],time[-1],'r', label="Average Temperature %2.2f °C"%(sum(temp)/len(temp)))
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = 3
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Temperature of SmartNode SoC CPU [°C]')
    plt.title(' SmartNode Instrument Core Temperature. Day %2d/%d/%4d'%(day[0],month[0],year[0]))
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)

def plot_power_day(plot_file_path, data_file_path):
    '''Función para generar un gráfico con las temperaturas del núcleo de la Raspberry Pi'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))
    # Extraemos los datos necesarios
    power = tools.read_data_from_json(data_file_path,"power [mW]")
    year = tools.read_data_from_json(data_file_path,"year")
    month = tools.read_data_from_json(data_file_path,"month")
    day = tools.read_data_from_json(data_file_path,"day")
    hour = tools.read_data_from_json(data_file_path,"hour")
    minutes = tools.read_data_from_json(data_file_path,"minutes")

    time = []
    power = [float(i) for i in power]
    year = [float(i) for i in year]
    month = [float(i) for i in month]
    day = [float(i) for i in day]
    hour = [float(i) for i in hour]
    minutes = [float(i) for i in minutes]
    # Generamos la lista con las horas disponibles
    for i in range(len(power)):
        time.append("%2dh:%2dm"%(hour[i],minutes[i]))


    plt.plot(time,power,'b-',label = "Power")
    plt.hlines(sum(power)/len(power),time[0],time[-1],'r', label="Average Power %2.2f mW"%(sum(power)/len(power)))
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = 3
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Power Consumption of SmartNode [mW]')
    plt.title(' SmartNode Instrument Power Consumption. Day %2d/%d/%4d'%(day[0],month[0],year[0]))
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)


def plot_current_day(plot_file_path, data_file_path):
    '''Función para generar un gráfico con las Corrientes que consume el nodo'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))
    # Extraemos los datos necesarios
    current = tools.read_data_from_json(data_file_path,"current [mA]")
    year = tools.read_data_from_json(data_file_path,"year")
    month = tools.read_data_from_json(data_file_path,"month")
    day = tools.read_data_from_json(data_file_path,"day")
    hour = tools.read_data_from_json(data_file_path,"hour")
    minutes = tools.read_data_from_json(data_file_path,"minutes")

    time = []
    current = [float(i) for i in current]
    year = [float(i) for i in year]
    month = [float(i) for i in month]
    day = [float(i) for i in day]
    hour = [float(i) for i in hour]
    minutes = [float(i) for i in minutes]
    # Generamos la lista con las horas disponibles
    for i in range(len(current)):
        time.append("%2dh:%2dm"%(hour[i],minutes[i]))


    plt.plot(time,current,'b-',label = "Current")
    plt.hlines(sum(current)/len(current),time[0],time[-1],'r', label="Average Current %2.2f mA"%(sum(current)/len(current)))
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = 3
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Current Consumption of SmartNode [mA]')
    plt.title(' SmartNode Instrument Current Consumption. Day %2d/%d/%4d'%(day[0],month[0],year[0]))
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)

def plot_voltage_day(plot_file_path, data_file_path):
    '''Función para generar un gráfico con la tensión del nodo'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))
    # Extraemos los datos necesarios
    voltaje = tools.read_data_from_json(data_file_path,"voltage [V]")
    year = tools.read_data_from_json(data_file_path,"year")
    month = tools.read_data_from_json(data_file_path,"month")
    day = tools.read_data_from_json(data_file_path,"day")
    hour = tools.read_data_from_json(data_file_path,"hour")
    minutes = tools.read_data_from_json(data_file_path,"minutes")

    time = []
    voltaje = [float(i) for i in voltaje]
    year = [float(i) for i in year]
    month = [float(i) for i in month]
    day = [float(i) for i in day]
    hour = [float(i) for i in hour]
    minutes = [float(i) for i in minutes]
    # Generamos la lista con las horas disponibles
    for i in range(len(voltaje)):
        time.append("%2dh:%2dm"%(hour[i],minutes[i]))


    plt.plot(time,voltaje,'b-',label = "Voltage")
    plt.hlines(sum(voltaje)/len(voltaje),time[0],time[-1],'r', label="Average Voltage %2.2f V"%(sum(voltaje)/len(voltaje)))
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = 3
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Voltage of SmartNode [V]')
    plt.title(' SmartNode Instrument Voltage. Day %2d/%d/%4d'%(day[0],month[0],year[0]))
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)


def map_rpi_temp_day(plot_file_path,data_file_path):
    '''Función para generar un mapa de calor de las temperaturas del nodo'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figuras
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    # Extraemos los datos necesarios de la fecha inicial
    temp = tools.read_data_from_json(data_file_path,"Temp. CPU [C]")
    year = tools.read_data_from_json(data_file_path,"year")
    month = tools.read_data_from_json(data_file_path,"month")
    day = tools.read_data_from_json(data_file_path,"day")
    hour = tools.read_data_from_json(data_file_path,"hour")
    minutes = tools.read_data_from_json(data_file_path,"minutes")

    time = []
    temp = [float(i) for i in temp]
    year = [float(i) for i in year]
    month = [float(i) for i in month]
    day = [float(i) for i in day]
    hour = [float(i) for i in hour]
    minutes = [float(i) for i in minutes]
    # Generamos la lista con las horas disponibles (Eje X)
    for i in range(len(temp)):
        time.append("%2dh:%2dm"%(hour[i],minutes[i]))
    plt.xticks(np.arange(144),labels=time,rotation=45,fontsize=7)

    # Generación de datos del Eje Y y e insertamos los datos en el mapa de calor
    ndays = 120
    ticks_y =[]
    temp_map = np.zeros((ndays,144)) #144 es el número de fragmentos de 10 minutos en un día
    current_day = tools.strip_date_from_path(data_file_path)
    for day in range(ndays):
        shifted_day = tools.shift_day(current_day[1:],-day)
        data_file_path = current_day[0]+tools.join_date_from_list(shifted_day)
        ticks_y.append("%02d/%02d/%d"%(shifted_day[2],shifted_day[1],shifted_day[0]))
        if (path.exists(data_file_path)):
            # Leemos los datos del día
            temp = tools.read_data_from_json(data_file_path,"Temp. CPU [C]")
            temp = [float(i) for i in temp]
            hour = tools.read_data_from_json(data_file_path,"hour")
            minutes = tools.read_data_from_json(data_file_path,"minutes")
            for i in range(len(hour)):
                idx = int((hour[i]*60+minutes[i])/10) # Calculamos el fragmento en minutos donde nos encontramos
                temp_map[day,idx] = temp[i]

    # Representamos el gráfico con los datos
    plt.jet()
    plt.imshow(temp_map, origin='xy', vmin=45 , vmax=70)
    plt.yticks(np.arange(ndays),labels=ticks_y,rotation = 0, fontsize = 7)
    plt.colorbar()

    plt.xlabel('Hours')
    plt.ylabel('Days')
    plt.title(' SmartNode Instrument Core Temperature Evolution.')

    # Mostramos menos ticks en el gráfico (Eje X)
    every_nth = 3
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    # Mostramos menos ticks en el gráfico (Eje X)
    every_nth = 2
    for n, label in enumerate(ax.yaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.savefig(plot_file_path, dpi=300)
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)
