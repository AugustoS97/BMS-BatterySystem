/*!
  LTC6803-2 Multicell Battery Monitor

@verbatim
  The LTC6803 is a 2nd generation multicell battery stack
  monitor that measures up to 12 series connected cells. The
  cell measurement range of -0.3V to 5V makes the LTC6803
  suitable for most battery chemistries.

  Using the LTC6803-2, multiple devices can be connected in
  parallel with one host processor connection for all
  devices.
@endverbatim

http://www.linear.com/product/LTC6803-2

http://www.linear.com/product/LTC6803-2#demoboards


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

Copyright 2015 Linear Technology Corp. (LTC)
***********************************************************/

//! @ingroup BMS
//! @{
//! @defgroup LTC68032 LTC6803-2: Multicell Battery Monitor
//! @}

/*! @file
    @ingroup LTC68032
    Library for LTC6803-2 Multicell Battery Monitor
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC68032.h"
#include <SPI.h>

/***************************************************************************
***********6803 Functions***************************************************
***************************************************************************/


//!Initializes the SPI port
void LTC6803_initialize()
{
  quikeval_SPI_connect();
  spi_enable(SPI_CLOCK_DIV16); // This will set the Linduino to have a 1MHz Clock

}




// Function that writes configuration of LTC6803-2/-3
void LTC6803_wrcfg(uint8_t total_ic,uint8_t config[][6])
{
  uint8_t BYTES_IN_REG = 6;
  uint8_t CMD_LEN = 4+7;
  uint8_t *cmd;
  uint16_t cfg_pec;
  uint8_t cmd_index; //command counter

  cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));
  for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
  {
    cmd[0] = 0x80 + current_ic;
    cmd[1] = pec8_calc(1,cmd);

    cmd[2] = 0x01;
    cmd[3] = 0xc7;

    cmd_index = 4;


    for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
    {
      cmd[cmd_index] = config[current_ic][current_byte];
      cmd_index = cmd_index + 1;
    }

    cfg_pec = pec8_calc(BYTES_IN_REG, &config[current_ic][0]);    // calculating the PEC for each ICs configuration register data
    cmd[cmd_index ] = (uint8_t)cfg_pec;
    cmd_index = cmd_index + 1;


    output_low(LTC6803_CS);
    spi_write_array(CMD_LEN, cmd);
    output_high(LTC6803_CS);
  }
  free(cmd);
}


//!Function that reads configuration of LTC6803-2/-3
int8_t LTC6803_rdcfg(uint8_t total_ic, //Number of ICs in the system
                     uint8_t r_config[][7] //A two dimensional array that the function stores the read configuration data.
                    )
{
  uint8_t BYTES_IN_REG = 7;

  uint8_t cmd[4];
  uint8_t *rx_data;
  int8_t pec_error = 0;
  uint8_t data_pec;
  uint8_t received_pec;

  rx_data = (uint8_t *) malloc((BYTES_IN_REG*total_ic)*sizeof(uint8_t));



  for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)       //executes for each LTC6803 in the daisy chain and packs the data
  {
    //into the r_config array as well as check the received Config data
    //for any bit errors

    cmd[0] = 0x80 + current_ic;
    cmd[1] = pec8_calc(1,cmd);
    cmd[2] = 0x02;
    cmd[3] = 0xCE;


    output_low(LTC6803_CS);
    spi_write_read(cmd, 4, rx_data, (BYTES_IN_REG*total_ic));
    output_high(LTC6803_CS);


    for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
    {
      r_config[current_ic][current_byte] = rx_data[current_byte];
    }

    received_pec =  r_config[current_ic][6];
    data_pec = pec8_calc(6, &r_config[current_ic][0]);
    if (received_pec != data_pec)
    {
      pec_error = -1;
    }
  }

  free(rx_data);
  return(pec_error);
}


//!Function that starts Cell Voltage measurement
void LTC6803_stcvad()
{
  output_low(LTC6803_CS);
  spi_write(0x10);
  spi_write(0xB0);
  output_high(LTC6803_CS);
}


//! Function that Temp channel voltage measurement
void LTC6803_sttmpad()
{
  output_low(LTC6803_CS);
  spi_write(0x30);
  spi_write(0x50);
  output_high(LTC6803_CS);
}



//!Function that reads Temp Voltage registers
int8_t LTC6803_rdtmp(uint8_t total_ic, uint16_t temp_codes[][3])
{
  int data_counter = 0;
  int pec_error = 0;
  uint8_t data_pec = 0;
  uint8_t received_pec = 0;
  uint8_t cmd[4];
  uint8_t *rx_data;
  rx_data = (uint8_t *) malloc((7)*sizeof(uint8_t));
  for (int i=0; i<total_ic; i++)
  {
    cmd[0] = 0x80 + i;
    cmd[1] = pec8_calc(1,cmd);
    cmd[2] = 0x0E;
    cmd[3] = 0xEA;
    output_low(LTC6803_CS);
    spi_write_read(cmd, 4,rx_data,6);
    output_high(LTC6803_CS);

    received_pec =  rx_data[5];
    data_pec = pec8_calc(5, &rx_data[0]);
    if (received_pec != data_pec)
    {
      pec_error = -1;
    }

    int cell_counter = 0;
    data_counter = 0;
    int temp,temp2;

    temp = rx_data[data_counter++];
    temp2 = (rx_data[data_counter]& 0x0F)<<8;
    temp_codes[i][0] = temp + temp2 -512;
    temp2 = (rx_data[data_counter++])>>4;
    temp =  (rx_data[data_counter++])<<4;
    temp_codes[i][1] = temp+temp2 -512;
    temp2 = (rx_data[data_counter++]);
    temp =  (rx_data[data_counter++]& 0x0F)<<8;
    temp_codes[i][2] = temp+temp2 -512;
  }
  free(rx_data);
  return(pec_error);
}


//! Function that reads Cell Voltage registers

uint8_t LTC6803_rdcv( uint8_t total_ic, uint16_t cell_codes[][12])
{
  int data_counter =0;
  int pec_error = 0;
  uint8_t data_pec = 0;
  uint8_t received_pec = 0;
  uint8_t *rx_data;
  uint8_t cmd[4];
  rx_data = (uint8_t *) malloc((19)*sizeof(uint8_t));

  for (int i=0; i<total_ic; i++)
  {
    cmd[0] = 0x80 + i;
    cmd[1] = pec8_calc(1,cmd);
    cmd[2] = 0x04;
    cmd[3] = 0xDC;
    output_low(LTC6803_CS);
    spi_write_read(cmd, 4,rx_data,19);
    output_high(LTC6803_CS);

    received_pec =  rx_data[18];
    data_pec = pec8_calc(18, &rx_data[0]);
    if (received_pec != data_pec)
    {
      pec_error = -1;
    }

    int cell_counter = 0;
    data_counter = 0;
    uint16_t temp,temp2;

    for (int k = 0; k<12; k=k+2)
    {
      temp = rx_data[data_counter++];
      temp2 = (uint16_t)(rx_data[data_counter]&0x0F)<<8;
      cell_codes[i][k] = temp + temp2 -512;
      temp2 = (rx_data[data_counter++])>>4;
      temp =  (rx_data[data_counter++])<<4;
      cell_codes[i][k+1] = temp+temp2 -512;
    }

  }
  free(rx_data);
  return(pec_error);
}

//!Function that calculates PEC byte

uint8_t pec8_calc(uint8_t len, uint8_t *data)
{

  uint8_t  remainder = 0x41;//PEC_SEED;


  /*
   * Perform modulo-2 division, a byte at a time.
   */
  for (int byte = 0; byte < len; ++byte)
  {
    /*
     * Bring the next byte into the remainder.
     */
    remainder ^= data[byte];

    /*
     * Perform modulo-2 division, a bit at a time.
     */
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      /*
       * Try to divide the current data bit.
       */
      if (remainder & 128)
      {
        remainder = (remainder << 1) ^ PEC_POLY;
      }
      else
      {
        remainder = (remainder << 1);
      }
    }
  }

  /*
   * The final remainder is the CRC result.
   */
  return (remainder);

}


//! Writes an array of bytes out of the SPI port
void spi_write_array(uint8_t len, // Option: Number of bytes to be written on the SPI port
                     uint8_t data[] //Array of bytes to be written on the SPI port
                    )
{
  for (uint8_t i = 0; i < len; i++)
  {
    spi_write((int8_t)data[i]);
  }
}


//!Writes and read a set number of bytes using the SPI port.
void spi_write_read(uint8_t tx_Data[],//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data arry
                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   )
{
  for (uint8_t i = 0; i < tx_len; i++)
  {
    spi_write(tx_Data[i]);

  }

  for (uint8_t i = 0; i < rx_len; i++)
  {
    rx_data[i] = (uint8_t)spi_read(0xFF);
  }

}
