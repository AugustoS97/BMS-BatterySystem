#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
*** Script para representar el ensayo de las baterías y calcular una función para estimar el SOC***
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

import csv
import numpy as np
import os
import estimate_SOC_tools as tools
import sys

FILE_NAME= 'DL_RecordData0.csv'
#FILE_PATH = '/home/augusto/Escritorio'
FILE_PATH = os.getcwd()[0:-4] #Se obtiene la ruta raiz de la carpeta src
internal_R = 0.066728 #V_oc - I*R = V_out. Circuito equivalente

SCRIPT_NAME = 'estimate_SOC_main.py'

if __name__ == '__main__':

    #Si se indica el nombre del fichero csv por consola
    if (len(sys.argv) > 1):
        FILE_NAME = sys.argv[1]
    if (len(sys.argv) > 2): #Puede indicarse otro grado al llamar al script
        degree = int(sys.argv[2])
    if (len(sys.argv) > 3):
        internal_R = float(sys.argv[3])
        print("Valor de la resistencia interna de las celdas es: %.4f Ohmios" %internal_R)

    current = np.array([])
    voltage = np.array([])
    resistance = np.array([])
    power = np.array([])
    capacity = np.array([])
    energy = np.array([])
    time = np.array([])

    '''Se lee el CSV y se almacenan los datos'''
    print("Tratando de leer el fichero " + FILE_NAME)
    with open(FILE_PATH+'/data/'+FILE_NAME, newline='') as File:
        reader = csv.reader(File)
        for row in reader:
            #current =float(row[0])
            current = np.append(current, float(row[0]))
            voltage = np.append(voltage, float(row[1]))
            resistance = np.append(resistance, float(row[2]))
            power = np.append(power, float(row[3]))
            capacity = np.append(capacity, float(row[4]))
            energy = np.append(energy, float(row[5]))
            seconds = int(row[6][5:7])*60 + int(row[6][8:11]) #Se calcula el tiempo en segundos
            time = np.append(time, seconds)

    '''Se calcula el Voc de todos los datos obtenidos'''
    Voc = tools.calc_Voc(internal_R, voltage, current)

    '''Se estima el SOC en función de las medidas de Capacidad (mAh) y la tension'''
    SOC = tools.calc_SOC(internal_R, capacity, voltage, current)

    '''Representación de la curva SOC vs Voc'''
    tools.plot_SOC_data(Voc, SOC, 'SOC_empirical', FILE_PATH + '/figures')

    '''Calculo de la curva que mejor se asemeje  mediante regresión polinómica'''
    #Se realiza un estudio de las curvas desde grado 1 hasta grado 9  polinomio
    degree_inf = 2
    degree_sup = 9
    tools.plot_polinomical_reg (Voc, SOC, degree_inf, degree_sup, 'SOC_polynomical_'+str(degree_inf)+'_to_'+str(degree_sup-1), FILE_PATH + '/figures')
    for degree in range (degree_inf, degree_sup):
        coefs= tools.calc_polinomical_reg (Voc, SOC, degree, 'SOC_polynomical_degree_'+str(degree), FILE_PATH + '/figures')
        tools.plot_my_curve(Voc, SOC, degree, coefs, 'SOC_regretion_degree_'+str(degree), FILE_PATH + '/figures')


    '''Se representa la curva del grado seleccionado frente a la curva empirica'''
    #Se selecciona la curva de grado 4 por defecto
    degree = 4
    if (len(sys.argv) > 2): #Puede indicarse otro grado al llamar al script
        degree = int(sys.argv[2])

    #coefs= tools.calc_polinomical_reg (Voc, SOC, degree, 'SOC_polynomical_degree_'+str(degree), FILE_PATH + '/figures')

    '''Se representa una curva calculada en otro ensayo frente a un Voc'''
    #tools.plot_my_curve(Voc, SOC, degree, coefs, 'SOC_my_curve_degree_'+str(degree), FILE_PATH + '/figures')
