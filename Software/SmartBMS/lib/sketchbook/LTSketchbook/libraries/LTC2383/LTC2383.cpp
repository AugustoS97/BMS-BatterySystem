/*!
LTC2381-16: 16-Bit, 250Ksps ADC.
LTC2382-16: 16-Bit, 500Ksps ADC.
LTC2383-16: 16-Bit, 1Msps ADC

@verbatim

The following parts (DUT) are pin-compatible, 16-bit A/D converters with serial I/O, and an internal reference:
LTC2381-16: 16-Bit, 250Ksps ADC.
LTC2382-16: 16-Bit, 500Ksps ADC.
LTC2383-16: 16-Bit, 1Msps ADC

@endverbatim

http://www.linear.com/product/LTC2381-16
http://www.linear.com/product/LTC2382-16
http://www.linear.com/product/LTC2383-16

http://www.linear.com/product/LTC2381-16#demoboards
http://www.linear.com/product/LTC2382-16#demoboards
http://www.linear.com/product/LTC2383-16#demoboards


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

//! @ingroup Analog_to_Digital_Converters
//! @{
//! @defgroup LTC2383 LTC2383: 16/18-Bit 1Msps ADC
//! @}

/*! @file
    @ingroup LTC2383
    Library for LTC2383: 16-Bit 1Msps ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2383.h"
#include <SPI.h>


// Reads from a SPI LTC2383-XX device that has no configuration word and a 20 bit output word in 2's complement format.
void LTC2383_read(uint8_t cs, uint16_t *ptr_adc_code)
{
  LT_union_int16_2bytes command;    // LTC2383 data and command
  command.LT_uint16 = 0;                // Set to zero, not necessary but avoids
  // random data in scope shots.

//Form a four byte object to hold four bytes of data
  LT_union_int16_2bytes data; //instantiate the union
  data.LT_byte[1] = 0;
  data.LT_byte[0] = 0;

  spi_transfer_block(cs,command.LT_byte,data.LT_byte,2);

  *ptr_adc_code = data.LT_int16;

  return;
}


// Calculates the voltage corresponding to an adc code in 2's complement, given the reference voltage (in volts)
float LTC2383_code_to_voltage(int16_t adc_code, float vref)
{
  float voltage;

  voltage = (float)adc_code;
  voltage = voltage / (pow(2,15)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)

  return(voltage);
}
