/*!
Linear Technology DC1778A Demonstration Board
LTC3883: Single Phase Step-Down DC/DC Controller with Digital Power System Management

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3883

http://www.linear.com/demo/DC177A


Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! @file
    @ingroup LTC3883
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
#include "LT_PMBus.h"

#define LTC3883_I2C_ADDRESS 0x4F

// Global variables
static uint8_t ltc3883_i2c_address;
static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc3883_i2c_address = LTC3883_I2C_ADDRESS;
  print_prompt();
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t res;
  uint8_t model[7];
  uint8_t *addresses = NULL;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        menu_1_basic_commands();                 // Print single-ended voltage menu
        break;
      case 2:
        pmbus->readModel(ltc3883_i2c_address, model);
        Serial.print(F("MODEL "));
        Serial.print((char *)model);
        Serial.println();
        break;
      case 3:
        res = pmbus->readPmbusRevision(ltc3883_i2c_address);
        Serial.println(res, HEX);
        break;
      case 4:
        pmbus->enablePec(ltc3883_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 5:
        pmbus->disablePec(ltc3883_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 6:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 7 :
        pmbus->resetGlobal();
        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }

}

// Function Definitions

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1778A Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the DC1778A demo board.                                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  1-Basic Commands\n"));
  Serial.print(F("  2-Model Number\n"));
  Serial.print(F("  3-Revision Number\n"));
  Serial.print(F("  4-PEC On\n"));
  Serial.print(F("  5-PEC Off\n"));
  Serial.print(F("  6-Bus Probe\n"));
  Serial.print(F("  7-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
//! @return void
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

//! Print all voltages
//! @return void
void print_all_voltages()
{
  float   voltage;

  voltage = pmbus->readVout(ltc3883_i2c_address, false);
  Serial.print(F("LTC3883 VOUT "));
  Serial.println(voltage, DEC);
}

//! Print all curents
//! @return void
void print_all_currents()
{
  float   current;

  current = pmbus->readIout(ltc3883_i2c_address, false);
  Serial.print(F("LTC3883 IOUT "));
  Serial.println(current, DEC);
}

//! Print all status bytes and words
//! @return void
void print_all_status()
{
  uint8_t b;
  uint16_t w;

  b = pmbus->readStatusByte(ltc3883_i2c_address);
  Serial.print(F("LTC3883 STATUS BYTE 0x"));
  Serial.println(b, HEX);
  w = pmbus->readStatusWord(ltc3883_i2c_address);
  Serial.print(F("LTC3883 STATUS WORD 0x"));
  Serial.println(w, HEX);
}

//! Sequence off then on
//! @return void
void sequence_off_on()
{
  pmbus->sequenceOffGlobal();
  delay (2000);
  pmbus->sequenceOnGlobal();
}

//! Margin high
//! @return void
void margin_high()
{
  pmbus->marginHighGlobal();
}

//! Margin low
//! @return void
void margin_low()
{
  pmbus->marginLowGlobal();
}

//! Go to nominal
//! @return void
void margin_off()
{
  pmbus->sequenceOnGlobal();
}

//! Display menu 1
//! @return void
void menu_1_basic_commands()
{
  uint8_t user_command;

  do
  {
    //! Displays the Read/Write menu
    Serial.print(F("\nRead/Write\n\n"));
    Serial.print(F("  1-Read All Voltages\n"));
    Serial.print(F("  2-Read All Currents\n"));
    Serial.print(F("  3-Read All Status\n"));
    Serial.print(F("  4-Sequence Off/On\n"));
    Serial.print(F("  5-Margin High\n"));
    Serial.print(F("  6-Margin Low\n"));
    Serial.print(F("  7-Margin Off\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    switch (user_command)
    {
      case 1:
        print_all_voltages();
        break;
      case 2:
        print_all_currents();
        break;
      case 3:
        print_all_status();
        break;
      case 4:
        sequence_off_on();
        break;
      case 5:
        margin_high();
        break;
      case 6:
        margin_low();
        break;
      case 7:
        margin_off();
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}


