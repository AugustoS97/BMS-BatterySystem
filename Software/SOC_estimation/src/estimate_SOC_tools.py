#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
*** Herramientas para script para representar el ensayo de las baterías y calcular una función para estimar el SOC***
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
from matplotlib import pyplot as plt
import numpy as np
import os
from os import path

SCRIPT_NAME = "estimate_SOC_tools.py"

'''Se estima el Voc según la Resistencia interna, la V en los bornes de la batería
y la corriente consumida'''
def calc_Voc(internal_R, voltage, current):
    Voc = internal_R * current + voltage
    return Voc


'''Se estima el SOC en función de las medidas de Capacidad (mAh) y la tension'''
def calc_SOC(internal_R, capacity, voltage, current):
    Voc = calc_Voc(internal_R, voltage, current)
    max_capacity = capacity[len(capacity)-1]
    SOC = (max_capacity - capacity)/max_capacity
    return SOC

'''Representación de la curva SOC vs Voc'''
def plot_SOC_data(Voc, SOC, PLOT_NAME, FILE_PATH): #Voc es el array de tensiones circuito Abierto
# y SOC es el array de estados de carga calculados de forma empirica
    W  = 15
    H = 12
    fig = plt.figure(figsize=(W,H))
    plt.title("Estado de Carga según Voltaje Circuito Abierto")
    plt.plot(Voc, SOC)
    plt.xticks(rotation=45,fontsize=9)
    plt.ylabel("SOC")
    plt.xlabel("Voc")
    plt.grid()
    plt.savefig(FILE_PATH+'/'+PLOT_NAME, dpi = 300)
    #plt.show()


'''Calculo de la curva que mejor se asemeje  mediante regresión polinómica'''
#Se realiza un estudio de las curvas con diferentes grados del polinomio
def plot_polinomical_reg (Voc, SOC, degree_inf, degree_sup, PLOT_NAME, FILE_PATH):
    sols = {}
    for degree in range(degree_inf,degree_sup):
      z = np.polyfit(Voc, SOC, degree, full=True)
      sols[degree] = z

    # Pintar datos empiricos
    W  = 15
    H = 12
    fig = plt.figure( figsize=(W,H))
    plt.plot(Voc, SOC, '-', label = "SOC empirica")

    # Pintar curvas de ajuste
    Vocp = np.arange(2.7, 4.15, 0.01)
    for degree, sol in sols.items():
      coefs, error, *_ = sol
      p = np.poly1d(coefs)
      #plt.plot(Vocp, p(Vocp), "--", label="Gr: %s. Error %.3f" % (degree, error) )
      plt.plot(Vocp, p(Vocp), "--", label="Regresión. Gr: %s" % (degree) )
      print('Coeficientes para grado ' + str(degree) + ' :\n')
      print(coefs)
      print('\n')

    plt.title("SOC Estimado Según Voltaje Circuito Abierto. Regresión desde grado "+ str(degree_inf)+ " hasta " + str(degree_sup-1))
    plt.xticks(rotation=45,fontsize=9)
    plt.ylabel("SOC")
    plt.xlabel("Voc")
    plt.grid()
    plt.legend()
    plt.savefig(FILE_PATH+'/'+PLOT_NAME, dpi = 300)
    #plt.show()

'''Se representa la curva del grado seleccionado frente a la curva empirica'''
def calc_polinomical_reg (Voc, SOC, degree, PLOT_NAME, FILE_PATH):
    sols = {}
    z = np.polyfit(Voc, SOC, degree, full=True)
    sols[degree-1] = z
    for i, sol in sols.items():
        coefs, error, *_ = sol #Se extraen los coeficientes del polinomio

    print('Coeficientes calculados para grado '+ str(degree) + ' :')
    print(coefs)
    print("")

    Voc_test = np.arange(2.7,4.2,0.01) #Se genera un array de Voc

    #Se calcula el SOC estimado en función del ajuste polinomial. Ejemplo para grado 5
    #SOC_pr = coefs[5] + Voc_test*coefs[4] + Voc_test**2* coefs[3]  + Voc_test**3 * coefs[2] +  Voc_test**4* coefs[1] + Voc_test **5 * coefs[0]
    #Parametrizamos el calculo del SOC estimado con la curva del grado indicado
    SOC_pr = np.zeros (len(Voc_test))
    expresion = ""
    for i in range (0,degree+1):
        SOC_pr = SOC_pr + (coefs[i]*(Voc_test**(degree - i)))
        expresion += (str(round(coefs[i],4))+"*V^(" + str(degree-i)+") + ")

    print("La expresion polinomica es:")
    print(expresion[0:-9])
    print("")

    W  = 15
    H = 12
    fig = plt.figure( figsize=(W,H))
    plt.title("SOC Estimado Según Voltaje Circuito Abierto. Curva de Grado " + str(degree))
    plt.xticks(rotation=45,fontsize=9)
    plt.ylabel("SOC")
    plt.xlabel("Voc")
    plt.plot(Voc, SOC, label="SOC empírico")
    plt.plot(Voc_test, SOC_pr, label = "SOC estimado con curva de grado " + str(degree))
    plt.grid()
    plt.legend()
    plt.savefig(FILE_PATH+'/'+PLOT_NAME, dpi = 300)
    #plt.show()
    return (coefs) #Devuelve los coeficientes de la funcion predicha

'''Se representa una curva arbitraria dada por sus coeficientes frente a la curva SOC empirica dada'''
def plot_my_curve(Voc, SOC, degree, coefs, PLOT_NAME, FILE_PATH):

    #Se calcula el SOC estimado en función del ajuste polinomial. Ejemplo para grado 5
    #SOC_pr = coefs[5] + Voc_test*coefs[4] + Voc_test**2* coefs[3]  + Voc_test**3 * coefs[2] +  Voc_test**4* coefs[1] + Voc_test **5 * coefs[0]
    #Parametrizamos el calculo del SOC estimado con la curva del grado indicado
    SOC_pr = np.zeros (len(Voc)) #SOC estimada con la regresion polinomica
    expresion = ""
    for i in range (0,degree+1):
        SOC_pr = SOC_pr + (coefs[i]*(Voc**(degree - i)))
        expresion += (str(round(coefs[i],4))+"*V^(" + str(degree-i)+") + ")

    print("La expresion polinomica es:")
    print(expresion[0:-9])
    print("")

    #Se calcula el error maximo cometido en la regresion
    SOC_dif = abs(SOC_pr - SOC)
    maximum_dif = np.amax(SOC_dif)
    where_max_dif = np.where(SOC_dif == maximum_dif)
    error_rel = maximum_dif /(SOC[where_max_dif]) # abs(SOC - SOC_pr)/(SOC)

    W  = 15
    H = 12
    fig = plt.figure( figsize=(W,H))
    plt.title("SOC Estimado Según Voltaje Circuito Abierto. Curva de Grado " + str(degree))
    plt.xticks(rotation=45,fontsize=9)
    plt.ylabel("SOC")
    plt.xlabel("Voc")
    plt.plot(Voc, SOC, label="SOC empírico")
    plt.plot(Voc, SOC_pr, label = "SOC estimado con curva de grado " + str(degree))
    plt.plot([Voc[where_max_dif],Voc[where_max_dif]], [SOC[where_max_dif],SOC_pr[where_max_dif]], 'o-', color='red', label ="Error relativo máximo = %.3f" %error_rel)
    plt.grid()
    plt.legend()
    plt.savefig(FILE_PATH+'/'+PLOT_NAME, dpi = 300)
    #plt.show()
