#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Representación de los datos procedentes de los csv del BMS

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
import bms_data_tools as tools
from os import path

SCRIPT_NAME = "bms_data_plot_tools.py"

def plot_bms_temp(plot_file_path, temp, day, month,year, hour, minutes, seconds, Ntemp):
    '''Función para generar un gráfico con las temperaturas de los termistores'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    #fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(temp)/Ntemp)):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    for temp_index in range(Ntemp):
        fig, ax = plt.subplots(figsize=(W,H))
        aux = np.array([], dtype = float)
        for i in range (0, int(len(temp)/Ntemp)):
            aux = np.append(aux, temp[i*Ntemp + temp_index])
        plt.plot(time,aux,'-',label = "Temperature Cell %d"%temp_index)
        plt.xticks(rotation = 45, fontsize = 7)

        # Mostramos menos ticks en el gráfico
        every_nth = round(len(temp)/50)
        for n, label in enumerate(ax.xaxis.get_ticklabels()):
            if n % every_nth != 0:
                label.set_visible(False)

        plt.hlines(sum(aux)/len(aux),time[0],time[-1],'r','--', label = "Average Temperature %2.2f °C"%(sum(aux)/len(aux)))
        plt.grid()
        plt.xlabel('Hours')
        plt.ylabel('Temperature of Cells [°C]')
        plt.title('Cells Temperature. Day ' + day + '/' + month +'/' + year + ' Termistor ' + str(temp_index))
        plt.legend()
        plt.savefig(plot_file_path + '_' + str(temp_index) + '.png', dpi=300)
        plt.show()
        plt.close()
        tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)

def plot_bms_all_temp(plot_file_path, temp, day, month,year, hour, minutes, seconds, Ntemp):
    '''Función para generar un gráfico con las temperaturas de los termistores'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(temp)/Ntemp)):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    for temp_index in range(Ntemp):
        #fig, ax = plt.subplots(figsize=(W,H))
        aux = np.array([], dtype = float)
        for i in range (0, int(len(temp)/Ntemp)):
            aux = np.append(aux, temp[i*Ntemp + temp_index])
        plt.plot(time,aux,'-',label = "Temperature Cell %d"%temp_index)
        plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = round(len(temp)/50)
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.hlines(sum(temp)/len(temp),time[0],time[-1],'r','--', label = "Average Temperature %2.2f °C"%(sum(temp)/len(temp)))
    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Temperature of Cells [°C]')
    plt.title('Cells Temperature. Day ' + day + '/' + month +'/' + year)
    plt.legend()
    plt.savefig(plot_file_path + '.png', dpi=300)
    plt.show()
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)



def plot_bms_cell_volt(plot_file_path, cellvolt, day, month,year, hour, minutes, seconds, Ncell):
    '''Función para generar un gráfico con los voltajes de celdas'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(cellvolt)/Ncell)):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    for cell_index in range(Ncell):
        aux = np.array([], dtype = float)
        for i in range (0, int(len(cellvolt)/Ncell)):
            aux = np.append(aux, cellvolt[i*Ncell + cell_index])
        plt.plot(time,aux,'-',label = "Cell Voltage %d"%cell_index)
        plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = round(len(seconds)/50)
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Cells Voltage [V]')
    plt.title('Cells Voltage. Day ' + day + '/' + month +'/' + year)
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.show()
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)

def plot_bms_dif_cell_volt(plot_file_path, cellvolt, day, month,year, hour, minutes, seconds, Ncell):
    '''Función para generar un gráfico con las diferencia de voltajes de celdas'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(cellvolt)/Ncell)):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    #Algoritmo para hacer la diferencia entre la mayor y menor
    max_diff_array = np.array([], dtype = float)
    for i in range (0, int(len(cellvolt)/Ncell)):
        aux = np.array([], dtype = float)
        for j in range (0, Ncell):
            aux = np.append(aux, cellvolt[j*Ncell + i])
        max_diff_array = np.append(max_diff_array, np.amax(aux) - np.amin(aux)) #Se obtiene la max diferencia de todas las celdas en un instante


    plt.plot(time,max_diff_array,'-',label = "Cells Voltage Difference")
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = round(len(seconds)/50)
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Cells Voltage Difference [V]')
    plt.title('Cells Voltage Difference. Day ' + day + '/' + month +'/' + year)
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.show()
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)

def plot_bms_dif_cell_volt_percent(plot_file_path, cellvolt, day, month,year, hour, minutes, seconds, Ncell):
    '''Función para generar un gráfico con las diferencia de voltajes de celdas en % relativo'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(cellvolt)/Ncell)):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    #Algoritmo para hacer la diferencia entre la mayor y menor
    max_diff_array = np.array([], dtype = float)
    for i in range (0, int(len(cellvolt)/Ncell)):
        aux = np.array([], dtype = float)
        for j in range (0, Ncell):
            aux = np.append(aux, cellvolt[j*Ncell + i])
        max_diff_array = np.append(max_diff_array, 100.0*(np.amax(aux) - np.amin(aux))/np.amin(aux)) #Se obtiene la max diferencia de todas las celdas en un instante en porcentaje


    plt.plot(time,max_diff_array,'-',label = "Cells Voltage Difference")
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = round(len(seconds)/50)
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Cells Voltage Difference [%]')
    plt.title('Cells Voltage Difference Percentage. Day ' + day + '/' + month +'/' + year)
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.show()
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)



def plot_bms_current(plot_file_path, current, day, month,year, hour, minutes, seconds):
    '''Función para generar un gráfico con la corriente consumida'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(current))):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    plt.plot(time, current,'-',label = "Current [A]")
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = round(len(seconds)/50)
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Current [A]')
    plt.title('Current Consumption. Day ' + day + '/' + month +'/' + year)
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.show()
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)



def plot_bms_soc(plot_file_path, soc, day, month,year, hour, minutes, seconds):
    '''Función para generar un gráfico con el SOC'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(soc))):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    plt.plot(time, soc,'-',label = "SOC")
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = round(len(seconds)/50)
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('SOC [%]')
    plt.title('State of Charge. Day ' + day + '/' + month +'/' + year)
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.show()
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)


def plot_bms_voltage(plot_file_path, voltage, day, month,year, hour, minutes, seconds):
    '''Función para generar un gráfico con el voltaje global de la batería'''
    tools.write_log(SCRIPT_NAME,"Comenzando la preparación del gráfico %s."%plot_file_path)
    # Propiedades de la Figura
    W = 16
    H = 12

    fig, ax = plt.subplots(figsize=(W,H))

    time = []

    # Generamos la lista con las horas disponibles
    for i in range(0, int(len(voltage))):
        time.append("%2dh:%2dm:%2ds"%(hour[i],minutes[i], seconds[i]))

    plt.plot(time, voltage,'-',label = "Battery Voltage [V]")
    plt.xticks(rotation = 45, fontsize = 7)

    # Mostramos menos ticks en el gráfico
    every_nth = round(len(seconds)/50)
    for n, label in enumerate(ax.xaxis.get_ticklabels()):
        if n % every_nth != 0:
            label.set_visible(False)

    plt.grid()
    plt.xlabel('Hours')
    plt.ylabel('Battery Voltage [V]')
    plt.title('Battery Voltage. Day ' + day + '/' + month +'/' + year)
    plt.legend()
    plt.savefig(plot_file_path, dpi=300)
    plt.show()
    plt.close()
    tools.write_log(SCRIPT_NAME,"El gráfico %s ha sido procesado."%plot_file_path)
