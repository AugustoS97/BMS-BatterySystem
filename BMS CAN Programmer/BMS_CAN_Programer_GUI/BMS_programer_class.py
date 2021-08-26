#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Software con Interfaz Gráfica para configurar y leer los datos del BMS por CAN

*** Definición de la clase ***

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


SCRIPT_NAME = "BMS_programer_class.py"

from BMS_programer_GUI import *
import BMS_programer_tools as tool
import serial
import time, os, threading
import numpy as np
from datetime import datetime
import csv

class MainWindow(QtWidgets.QMainWindow, Ui_BMS_Programmer_UI):
    def __init__(self, *args, **kwargs):
        QtWidgets.QMainWindow.__init__(self, *args, **kwargs)
        self.setupUi(self)
        #Definición de variables
        self._CELL_N = 4
        self._CELL_TEMP_N = 4
        self._BAT_VOLT = 0.0
        self._SOC = 0.0
        self._SOH = 0.0
        self._VUV = 0
        self._VOV = 0
        self._TIME_SLEEP = 0
        self._max_volt_diff = 0
        self._current = 0.0
        self._continue_recording = False
        self._cell_volt = np.zeros(12, dtype = float)
        self._cell_temp = np.zeros(32, dtype = float)
        self._cell_balancing_groupA = 0 #Numero que indica que celdas entre 1 y 8 se están balanceando
        self._cell_balancing_groupB = 0 #Numero que indica que celdas entre 9 y 12 se están balanceando

        #Definición de métodos
        self.connect_button.clicked.connect(self.connect_device)
        self.disconnect_button.clicked.connect(self.disconnect_device)
        self.read_config_button.clicked.connect(self.read_config)
        self.record_button.clicked.connect(self.record_data)
        self.stop_button.clicked.connect(self.stop_record_data)
        self.apply_VUV.clicked.connect(self.config_VUV)
        self.apply_VOV.clicked.connect(self.config_VOV)
        self.apply_DCTO.clicked.connect(self.config_DCTO)
        self.apply_diff_volt.clicked.connect(self.config_max_volt_diff)
        self.apply_cell_number.clicked.connect(self.config_cell_number)
        self.apply_cell_number_parallel.clicked.connect(self.config_cell_number_parallel)
        self.apply_cell_temp_number.clicked.connect(self.config_cell_temp_number)
        self.apply_bal_mode.clicked.connect(self.config_balancing_mode)
        self.apply_time_sleep.clicked.connect(self.config_t_sleep)
        self.apply_current_offset.clicked.connect(self.config_current_offset)
        self.force_balancing_button.clicked.connect(self.force_balance)

    ''' Funcion para conectarse por serial al dispositivo '''
    def connect_device(self):
        try:
            self.serial_obj = serial.Serial('/dev/'+str(self.device_select.currentText()),baudrate=115200, timeout = 1.0)
            self.thread_serial = threading.Thread(target = read_serial, args=(self,), daemon = True)
            self.thread_serial.start()
            self.thread_update = threading.Thread(target = update_config, args = (self,), daemon = True)
            self.thread_update.start()
            self.read_config()
            self.statusbar.showMessage('Serial Port correctly opened')
        except:
            self.statusbar.showMessage('Error opening selected Serial Port')
            tool.print_log("Error opening selected Serial Port")

    ''' Funcion para cerrar el puerto serial '''
    def disconnect_device(self):
        try:
            self.serial_obj.close()
            self.statusbar.showMessage('Serial Port closed correctly')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para actualizar todas las configuraciones '''
    def read_config(self):
        try:
            if self.serial_obj.isOpen():
                self.serial_obj.write('Z1\n'.encode('ASCII'))
                self.statusbar.showMessage('Asking config to BMS')
                tool.print_log('Asking for config to BMS. Msg: Z1')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log('Serial Port not open')
        return

    ''' Función para escribir los valores de los voltajes '''
    def write_voltages (self):
        return

    ''' Función para empezar la grabación en un CSV '''
    def record_data(self):
        filepath = "/home/" + self.filename_input.text()
        try:
            self.statusbar.showMessage('Starting recording data')
            tool.print_log('Starting recording data on ' + filepath)
            self.thread_write_csv = threading.Thread(target = write_csv, args = (self, filepath,self._cell_volt, self._CELL_N, self._cell_temp, self._CELL_TEMP_N, self._current, self._BAT_VOLT, self._SOC, self._SOH, self._cell_balancing_groupA, self._cell_balancing_groupB, ), daemon = True)
            self.thread_write_csv.start()
            self._continue_recording = True #Se pone a 1 el flag que indica que se graben datos
        except:
            self.statusbar.showMessage('Error recording data')
            tool.print_log("Error recording data")
        return

    ''' Función para parar la grabación de datos del CSV y cerrarlo '''
    def stop_record_data(self):
        try:
            self._continue_recording = False #Se pone a cero el flag que indica que se graben datos
            self.data_csv.close()
            self.statusbar.showMessage('Record data finished')
            tool.print_log('Record data finished')
        except:
            self.statusbar.showMessage('CSV file was not created. It is impossible to close')
            tool.print_log('CSV file was not created. It is impossible to close')
        return

    ''' Función para configurar el valor de voltaje mínimo de una celda '''
    def config_VUV(self):
        VUV = int(self.VUV_value.value()/0.02)
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['VUV_ID'])+str(VUV)+'\n').encode('ASCII'))
               tool.print_log('Sending Undervoltage value config. Msg: '+str(tool.id_serial['VUV_ID'])+str(VUV))
               self.statusbar.showMessage('Writing Unvervoltage Cell value')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el valor de voltaje máximo de una celda '''
    def config_VOV(self):
        VOV = int(self.VOV_value.value()/0.02)
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['VOV_ID'])+str(VOV)+'\n').encode('ASCII'))
               tool.print_log('Sending Overvoltage value config. Msg: '+str(tool.id_serial['VOV_ID'])+str(VOV))
               self.statusbar.showMessage('Writing Overvoltage Cell value')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el TIMEOUT de balanceo (DCTO). Ver datasheet LTC 6804 '''
    def config_DCTO(self):
        DCTO = self.DCTO_value.currentIndex() #El indice del elemento escogio coincide
        #con el comando DCTO a enviar (0=disabled, 1=0.5 min, 2=1 min...)
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['DCTO_ID'])+str(DCTO)+'\n').encode('ASCII'))
               tool.print_log('Sending DCTO value config. Msg: '+ str(tool.id_serial['DCTO_ID'])+str(DCTO))
               self.statusbar.showMessage('Writing DCTO value')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar la máxima diferencia de tensión admisible entre celdas '''
    def config_max_volt_diff(self):
        max_diff_volt = int(self.diff_volt_value.value())
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['MAX_DIFF_V_ID'])+str(max_diff_volt)+'\n').encode('ASCII'))
               tool.print_log('Sending Maximun Voltage Difference config. Msg: '+ str(tool.id_serial['MAX_DIFF_V_ID'])+str(max_diff_volt))
               self.statusbar.showMessage('Writing maximun voltage cell difference')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el número de celdas conectadas en serie'''
    def config_cell_number(self):
        cell_n = int(self.cell_number.value())
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['CELL_N_ID'])+str(cell_n)+'\n').encode('ASCII'))
               tool.print_log('Sending cells number config. Msg: '+ str(tool.id_serial['CELL_N_ID'])+str(cell_n))
               self.statusbar.showMessage('Writing Cell number')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el número de celdas conectadas en paralelo'''
    def config_cell_number_parallel(self):
        cell_n_parallel = int(self.cell_number_parallel.value())
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['CELL_PARALLEL_N_ID'])+str(cell_n_parallel)+'\n').encode('ASCII'))
               tool.print_log('Sending cells number in parallel config. Msg: '+ str(tool.id_serial['CELL_PARALLEL_N_ID'])+str(cell_n_parallel))
               self.statusbar.showMessage('Writing Cell number in parallel')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el número de sensores de temperatura activos '''
    def config_cell_temp_number(self):
        cell_temp_N = int(self.cell_temp_number.value())
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['CELL_TEMP_N_ID'])+str(cell_temp_N)+'\n').encode('ASCII'))
               tool.print_log('Sending Temp Sensor number config. Msg: '+ str(tool.id_serial['CELL_TEMP_N_ID'])+str(cell_temp_N))
               self.statusbar.showMessage('Writing Temperature Sensor number')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el tipo de balanceo del BMS '''
    def config_balancing_mode(self):
        bal_mode = self.balancing_mode_value.currentIndex() #El indice del elemento escogido coincide
        #con el comando de tipo de balanceo a enviar (0=disabled, 1=solo en carga, 2=solo descarga...)
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['BAL_MODE_ID'])+str(bal_mode)+'\n').encode('ASCII'))
               tool.print_log('Sending Balance Mode config. Msg:'+ str(tool.id_serial['BAL_MODE_ID'])+str(bal_mode))
               self.statusbar.showMessage('Writing Balance Mode configuration')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el tiempo de reposo del BMS '''
    def config_t_sleep(self):
        time_sleep = int(self.time_sleep_value.value())
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['T_SLEEP_ID'])+str(time_sleep)+'\n').encode('ASCII'))
               tool.print_log('Sending time to Sleep value config. Msg: '+ str(tool.id_serial['T_SLEEP_ID'])+str(time_sleep))
               self.statusbar.showMessage('Writing Time to Sleep value')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para configurar el offset del sensor de corriente '''
    def config_current_offset(self):
        current_offset = int(self.current_offset_value.value())
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['CURRENT_OFFSET_ID'])+str(current_offset)+'\n').encode('ASCII'))
               tool.print_log('Sending Current Offset (mA) value config. Msg: '+ str(tool.id_serial['CURRENT_OFFSET_ID'])+str(current_offset))
               self.statusbar.showMessage('Writing Current Offset value')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")
        return

    ''' Función para forzar el balanceo de las celdas elegidas '''
    def force_balance(self):
        #Se crea un byte con los bit a 1 correspondiente a las celdas a balancear
        bal_1_8 = int((self.sel_bal_8.isChecked()<<7)|(self.sel_bal_7.isChecked()<<6)\
                |(self.sel_bal_6.isChecked()<<5)|(self.sel_bal_5.isChecked()<<4)\
                |(self.sel_bal_4.isChecked()<<3)|(self.sel_bal_3.isChecked()<<2)\
                |(self.sel_bal_2.isChecked()<<1)|(self.sel_bal_1.isChecked()))
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['FORCE_BAL_1_8'])+str(bal_1_8)+'\n').encode('ASCII'))
               tool.print_log('Forcing balance cell 1 to 8. Msg: '+str(tool.id_serial['FORCE_BAL_1_8'])+str(bal_1_8))
            self.statusbar.showMessage('Writing cells forced to balance')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")

        bal_9_12 = int((self.sel_bal_12.isChecked()<<3)|(self.sel_bal_11.isChecked()<<2)\
                |(self.sel_bal_10.isChecked()<<1)|(self.sel_bal_9.isChecked()))
        try:
            if self.serial_obj.isOpen():
               self.serial_obj.write((str(tool.id_serial['FORCE_BAL_9_12'])+str(bal_9_12)+'\n').encode('ASCII'))
               tool.print_log('Forcing balancing cell 9 to 12. Msg: '+str(tool.id_serial['FORCE_BAL_9_12'])+str(bal_9_12))
               self.statusbar.showMessage('writing cells forced to balance')
        except:
            self.statusbar.showMessage('Serial Port not opened')
            tool.print_log("Serial Port not open")

        return

    ''' Funcion para refrescar los datos de tensiones, temperaturas y demás parametros '''
    def refresh_values(self):
        return

''' Funcion para pedir de forma periódica los datos de config '''
def update_config(self):
    while True:
        self.read_config()
        self._BAT_VOLT = get_total_volt(self, self._CELL_N, self._cell_volt)
        self.voltage_value.setText(str(self._BAT_VOLT))
        time.sleep(8)
    return

''' Función para leer de forma cíclica valores por el puerto Serial y actualizarlos en la interfaz '''
def read_serial(self):
    tool.print_log("Reading from Serial Port")
    try:
        with self.serial_obj:
            while True:
                ID=''
                try:
                    line = self.serial_obj.readline().decode('ASCII')
                    #print(line)
                    if(len(line)>0):
                        ID = line[0]
                        if(ID == str(tool.id_serial['VCELL_GROUP_A'])): #Se reciben V celdas 1 a 4
                            if(line[1] == '1'):
                                Vcell1 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[0] = Vcell1
                                self.cell1_info.setText(str(Vcell1) +' V')
                            if(line[1] == '2'):
                                Vcell2 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[1] = Vcell2
                                self.cell2_info.setText(str(Vcell2) +' V')
                            if(line[1] == '3'):
                                Vcell3 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[2] = Vcell3
                                self.cell3_info.setText(str(Vcell3) +' V')
                            if(line[1] == '4'):
                                Vcell4 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[3] = Vcell4
                                self.cell4_info.setText(str(Vcell4) +' V')

                        if(ID == str(tool.id_serial['VCELL_GROUP_B'])): #Se reciben V celdas 5 a 8
                            if(line[1] == '1'):
                                Vcell5 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[4] = Vcell5
                                self.cell5_info.setText(str(Vcell5) +' V')
                            if(line[1] == '2'):
                                Vcell6 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[5] = Vcell6
                                self.cell6_info.setText(str(Vcell6) +' V')
                            if(line[1] == '3'):
                                Vcell7 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[6] = Vcell7
                                self.cell7_info.setText(str(Vcell7) +' V')
                            if(line[1] == '4'):
                                Vcell8 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[8] = Vcell8
                                self.cell8_info.setText(str(Vcell8) +' V')

                        if(ID == str(tool.id_serial['VCELL_GROUP_C'])): #Se reciben V celdas 9 a 12
                            if(line[1] == '1'):
                                Vcell9 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[8] = Vcell9
                                self.cell9_info.setText(str(Vcell9) +' V')
                            if(line[1] == '2'):
                                Vcell10 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[9] = Vcell10
                                self.cell10_info.setText(str(Vcell10) +' V')
                            if(line[1] == '3'):
                                Vcell11 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[10] = Vcell11
                                self.cell11_info.setText(str(Vcell11) +' V')
                            if(line[1] == '4'):
                                Vcell12 = round(float(line[2:-1])*0.0001,4)
                                self._cell_volt[11] = Vcell12
                                self.cell12_info.setText(str(Vcell12) +' V')

                        if(ID == str(tool.id_serial['TEMP_VALUE_ID'])): #Se reciben las Temp
                            if(line[1] == '0' and line[2] == '1'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[0]=Temp
                                self.temp1_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '2'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[1]=Temp
                                self.temp2_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '3'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[2]=Temp
                                self.temp3_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '4'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[3]=Temp
                                self.temp4_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '5'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[4]=Temp
                                self.temp5_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '6'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[5]=Temp
                                self.temp6_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '7'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[6]=Temp
                                self.temp7_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '8'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[7]=Temp
                                self.temp8_info.setText(str(Temp) +' ºC')
                            if(line[1] == '0' and line[2] == '9'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[8]=Temp
                                self.temp9_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '0'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[9]=Temp
                                self.temp10_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '1'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[10]=Temp
                                self.temp11_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '2'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[11]=Temp
                                self.temp12_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '3'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[12]=Temp
                                self.temp13_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '4'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[13]=Temp
                                self.temp14_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '5'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[14]=Temp
                                self.temp15_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '6'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[15]=Temp
                                self.temp16_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '7'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[16]=Temp
                                self.temp17_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '8'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[17]=Temp
                                self.temp18_info.setText(str(Temp) +' ºC')
                            if(line[1] == '1' and line[2] == '9'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[18]=Temp
                                self.temp19_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '0'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[19]=Temp
                                self.temp20_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '1'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[20]=Temp
                                self.temp21_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '2'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[21]=Temp
                                self.temp22_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '3'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[22]=Temp
                                self.temp23_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '4'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[23]=Temp
                                self.temp24_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '5'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[24]=Temp
                                self.temp25_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '6'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[25]=Temp
                                self.temp26_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '7'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[26]=Temp
                                self.temp27_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '8'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[27]=Temp
                                self.temp28_info.setText(str(Temp) +' ºC')
                            if(line[1] == '2' and line[2] == '9'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[28]=Temp
                                self.temp29_info.setText(str(Temp) +' ºC')
                            if(line[1] == '3' and line[2] == '0'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[29]=Temp
                                self.temp30_info.setText(str(Temp) +' ºC')
                            if(line[1] == '3' and line[2] == '1'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[30]=Temp
                                self.temp31_info.setText(str(Temp) +' ºC')
                            if(line[1] == '3' and line[2] == '2'):
                                Temp = round((float(line[3:-1])-5)*0.3,2)
                                self._cell_temp[31]=Temp
                                self.temp32_info.setText(str(Temp) +' ºC')

                        if(ID == str(tool.id_serial['CURRENT_VALUE_ID'])): #Se recibe la corriente
                            current = round(float(line[1:-1])*0.001,3)
                            self._current = current #En amperios
                            self.current_value.setText(str(current))

                        if(ID == str(tool.id_serial['SOC_VALUE_ID'])): #Se recibe el SOC
                            SOC = round(int(line[1:-1])*0.01,2)
                            if ((SOC <= 100) and (SOC >= 1)):
                                self._SOC = SOC
                                #self.SOC_value.setValue(int(SOC))


                        ''' SE MUESTRAN LOS VALORES DE CONFIGURACIÓN'''
                        if(ID == str(tool.id_serial['VUV_ID'])): #Se recibe el VUV
                            VUV = round(float(line[1:-1])*0.02)
                            self._VUV = VUV
                            self.VUV_value.setValue(VUV)
                        if(ID == str(tool.id_serial['VOV_ID'])): #Se recibe el VOV
                            VOV = round(float(line[1:-1])*0.02)
                            self._VOV = VOV
                            self.VOV_value.setValue(VOV)
                        if(ID == str(tool.id_serial['MAX_DIFF_V_ID'])): #Se recibe la máxima dif de V entre celdas
                            MAX_DIFF_V = int(line[1:-1])
                            self.diff_volt_value.setValue(MAX_DIFF_V)
                        if(ID == str(tool.id_serial['CELL_N_ID'])): #Se recibe el nº celdas
                            CELL_N = int(line[1:-1])
                            if(CELL_N <= 12):
                                self._CELL_N = CELL_N
                            self.cell_number.setValue(CELL_N)
                        if(ID == str(tool.id_serial['T_SLEEP_ID'])): #Se recibe el tiempo de reposo
                            TSLEEP = int(line[1:-1])
                            if(TSLEEP <= 220):
                                self.time_sleep_value.setValue(TSLEEP)
                        if(ID == str(tool.id_serial['CURRENT_OFFSET_ID'])): #Se recibe el offset de corriente
                            CURRENT_OFFSET = int(line[1:-1])
                            self.current_offset_value.setValue(CURRENT_OFFSET)
                        if(ID == str(tool.id_serial['CELL_PARALLEL_N_ID'])): #Se recibe el nº de celdas en paralelo
                            cell_n_parallel = int(line[1:-1])
                            if(cell_n_parallel <= 100):
                                self.cell_number_parallel.setValue(cell_n_parallel)
                        if(ID == str(tool.id_serial['DCTO_ID'])): #Se recibe el valor de DCTO
                            DCT_value_conf = int(line[1:-1])
                            #self.DCTO_value.setCurrentIndex(DCTO_value_conf) #Se pone el valor de DCTO elegido en el ComboBox
                        if(ID == str(tool.id_serial['BAL_MODE_ID'])): #Se recibe el valor del Balancing Mode
                            balancing_mode_value_conf = int(line[1:-1])
                            self.balancing_mode_value.setCurrentIndex(balancing_mode_value_conf) #Se pone el valor del BalancingMode elegido en el ComboBox
                        if(ID == str(tool.id_serial['CELL_TEMP_N_ID'])): #Se recibe el nº de sensores Temp
                            CELL_TEMP_N = int(line[1:-1])
                            self._CELL_TEMP_N = CELL_TEMP_N
                            self.cell_temp_number.setValue(CELL_TEMP_N)
                        if(ID == str(tool.id_serial['FORCE_BAL_1_8'])): #Se recibe las celdas actualmente balanceandose
                            balance_1_8 = int(line[1:-1])
                            self._cell_balancing_groupA = balance_1_8 #Se actualiza el valor en la clase
                            if(balance_1_8 & 1): #Se modifica el color de la celd asegun se balancee o no
                                self.cell1_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell1_info.setStyleSheet("QLabel { color: black}")
                            if(balance_1_8 & 2):
                                self.cell2_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell2_info.setStyleSheet("QLabel { color: black}")
                            if(balance_1_8 & 4):
                                self.cell3_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell3_info.setStyleSheet("QLabel { color: black}")
                            if(balance_1_8 & 8):
                                self.cell4_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell4_info.setStyleSheet("QLabel { color: black}")
                            if(balance_1_8 & 16):
                                self.cell5_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell5_info.setStyleSheet("QLabel { color: black}")
                            if(balance_1_8 & 32):
                                self.cell6_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell6_info.setStyleSheet("QLabel { color: black}")
                            if(balance_1_8 & 64):
                                self.cell7_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell7_info.setStyleSheet("QLabel { color: black}")
                            if(balance_1_8 & 128):
                                self.cell8_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell8_info.setStyleSheet("QLabel { color: black}")

                        if(ID == str(tool.id_serial['FORCE_BAL_9_12'])): #Se recibe las celdas actualmente balanceandose
                            balance_9_12 = int(line[1:-1])
                            self._cell_balancing_groupB = balance_9_12 #Se actualiza el valor en la clase
                            #tool.print_log(balance_9_12)
                            if(balance_9_12 & 1):
                                self.cell9_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell9_info.setStyleSheet("QLabel { color: black}")
                            if(balance_9_12 & 2):
                                self.cell10_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell10_info.setStyleSheet("QLabel { color: black}")
                            if(balance_9_12 & 4):
                                self.cell11_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell11_info.setStyleSheet("QLabel { color: black}")
                            if(balance_9_12 & 8):
                                self.cell12_info.setStyleSheet("QLabel { color: red}")
                            else:
                                self.cell12_info.setStyleSheet("QLabel { color: black}")


                except KeyboardInterrupt:
                    tool.print_log("Exiting")
                    self.serial_obj.close()
    except:
        tool.print_log("Serial Port is closed")

    return

''' Función para calcular el voltaje total de la batería '''
def get_total_volt(self, Ncell, volt_array):
    total_volt = 0.0
    for i in range (0, Ncell): #Suma solo las celdas activas en la configuracion
        total_volt +=  volt_array[i]
    return round(total_volt,3)

''' Función para escribir los datos recibidos en un CSV '''
def write_csv(self, filepath, voltage_array, Ncell, temperature_array, Ntemp, current, battery_volt, SOC, SOH, cell_bal_1to8, cell_bal_9to12):
    try:
        with open(filepath, 'w') as self.data_csv:
            self.writer_csv = csv.writer(self.data_csv, delimiter = ',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
            header = ['Date and Time [dd/mm/yyyy/hh/mm/ss]', 'Ncell', 'NTemp', 'Current [A]']
            if (Ncell == 0):
                Ncell = 4
            for cell in range (1,Ncell + 1):
                header.append('Cell '+str(cell) +' [V]') #Se añaden los encabezados de las celdas
            header.append('Cell Balancing (Cell12|..|Cell1)') #Se añade el encabezado de las celdas balanceandose en ese instante
            if (Ntemp == 0):
                Ntemp = 4
            for temperature in range (1, Ntemp + 1):
                header.append('Temp Cell '+str(temperature)+ ' [C]') #Se añaden los encabezados de las temperaturas
            header.append ('SOC') #Se añade el encabezado del SOC
            header.append ('SOH')
            header.append ('Bat. Volt')
            self.writer_csv.writerow(header)
            self.statusbar.showMessage('CSV file opened correctly')
            tool.print_log('CSV file opened correctly')
            while self._continue_recording:
                try:
                    now= datetime.now()
                    dt_string = now.strftime("%d/%m/%Y/%H/%M/%S")
                    #date_time = str(now.day)+'/'+str(now.month)+'/'+str(now.year)+'/'+str(now.hour)+'/'+str(now.minute)+'/'+str(now.second)
                    line = [dt_string, str(Ncell), str(Ntemp), str(self._current)]
                    for cell in range (0,Ncell): #Se añaden solo los valores de V de las celdas activas
                        line.append(str(voltage_array[cell]))

                    #Se añaden las celdas balanceandose
                    cell_groupA = str(bin(cell_bal_1to8))[2:len(str(bin(cell_bal_1to8)))] #Se elimina el '0b' del comienzo
                    while len(cell_groupA) < 8: #Se obtiene un string de 8 caracteres
                        cell_groupA = '0' + cell_groupA
                    cell_groupB = str(bin(cell_bal_9to12))[2:len(str(bin(cell_bal_9to12)))]
                    while len(cell_groupB) < 4: #Se obtiene un string de 4 caracteres (Las 4 celdas del grupo B)
                        cell_groupB = '0'+ cell_groupB
                    cell_balancing = cell_groupB + cell_groupA  #Se obtiene la cadena binaria de las celdas que están actualmente balanceandose
                    #El formato es en binario Celda12|Celda 11|...|Celda 1
                    line.append(cell_balancing)

                    #Se añaden las temperaturas
                    for temperature in range (0, Ntemp): #Se añaden los valores de las Tª
                        line.append(str(temperature_array[temperature]))

                    line.append (str(SOC)) #Se añade el valor del SOC
                    line.append (str(SOH)) #Se añade el valor del SOH
                    line.append (str(battery_volt)) #Se añade el valor de la tensión de la batería

                    self.writer_csv.writerow(line)
                    self.statusbar.showMessage('CSV updated')
                    tool.print_log('CSV updated')
                    #data_csv.close()
                    time.sleep(10)
                except:
                    self._continue_recording = False
                    self.data_csv.close()
                    self.statusbar.showMessage('CSV closed. It was an error')
                    tool.print_log('CSV closed. It was an error')
    except:
        self.statusbar.showMessage('Error creating CSV file. Check the filepath')
        tool.print_log('Error creating CSV file. Check the filepath')
