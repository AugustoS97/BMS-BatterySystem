#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Software con Interfaz Gráfica para configurar y leer los datos del BMS por CAN

*** Script del programa principal ***

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

SCRIPT_NAME = "BMS_programer.py"

from BMS_programer_GUI import *
import BMS_programer_tools as tool
import BMS_programer_class as GUI



''' Programa principal '''
if __name__ == "__main__":
    tool.print_log('Opening GUI')
    app = GUI.QtWidgets.QApplication([])
    window = GUI.MainWindow()
    window.show()
    app.exec_()
