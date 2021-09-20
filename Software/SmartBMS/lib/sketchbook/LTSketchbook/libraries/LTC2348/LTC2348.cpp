/*!
LTC2348-18: Octal, 18-Bit, 200ksps Differential ±10.24V Input SoftSpan ADC with Wide Input Common Mode Range.
LTC2348-16: Octal, 16-Bit, 200ksps Differential ±10.24V Input SoftSpan ADC with Wide Input Common Mode Range.
LTC2344-18: Quad, 18-Bit, 400ksps/ch Differential SoftSpan ADC with Wide Input Common Mode Range.
LTC2344-16: Quad, 16-Bit, 400ksps/ch Differential SoftSpan ADC with Wide Input Common Mode Range.
LTC2333-18: Buffered 8-Channel, 18-Bit, 800ksps Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2333-16: Buffered 8-Channel, 16-Bit, 800ksps Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2357-18: Buffered Quad, 18-Bit, 350ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2357-16: Buffered Quad, 16-Bit, 350ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2358-18: Buffered Octal, 18-Bit, 200ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2358-16: Buffered Octal, 16-Bit, 200ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2353-18: Buffered Dual, 18-Bit, 550ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2353-16: Buffered Dual, 16-Bit, 550ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.


@verbatim

The LTC2348 is an 18-bit,low noise 8-channel simultaneous sampling successive
approximation register(SAR) ADC with differential,wide common mode range
inputs. Operating from a 5V low voltage supply, flexible high voltage supplies,
and using the internal reference and buffer, each channel of this SoftSpan ADC
can be independently configured on a conversion-by-conversion basis to accept
±10.24V, 0V to 10.24V, ±5.12V,or 0V to 5.12V signals.

@endverbatim

http://www.linear.com/product/LTC2348-18
http://www.linear.com/product/LTC2348-16
http://www.linear.com/product/LTC2344-18
http://www.linear.com/product/LTC2344-16
http://www.linear.com/product/LTC2333-18
http://www.linear.com/product/LTC2333-16
http://www.linear.com/product/LTC2357-18
http://www.linear.com/product/LTC2357-16
http://www.linear.com/product/LTC2358-18
http://www.linear.com/product/LTC2358-16
http://www.linear.com/product/LTC2353-18
http://www.linear.com/product/LTC2353-16

http://www.linear.com/product/LTC2348-18#demoboards
http://www.linear.com/product/LTC2348-16#demoboards
http://www.linear.com/product/LTC2344-18#demoboards
http://www.linear.com/product/LTC2344-16#demoboards
http://www.linear.com/product/LTC2333-18#demoboards
http://www.linear.com/product/LTC2333-16#demoboards
http://www.linear.com/product/LTC2357-18#demoboards
http://www.linear.com/product/LTC2357-16#demoboards
http://www.linear.com/product/LTC2358-18#demoboards
http://www.linear.com/product/LTC2358-16#demoboards
http://www.linear.com/product/LTC2353-18#demoboards
http://www.linear.com/product/LTC2353-16#demoboards


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
//! @defgroup LTC23XX LTC23XX: 16/18-Bit Differential Input SoftSpan ADC with Wide Input Common Mode Range.
//! @}

/*! @file
    @ingroup LTC23XX
    Library for LTC23XX: 16/18-Bit Differential Input SoftSpan ADC with Wide Input Common Mode Range.
*/
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2348.h"
#include "LT_SPI.h"

void LTC23XX_create_config_word(uint8_t channel, uint8_t config_number, uint32_t *config_word)
{
  *config_word = *config_word | (uint32_t(config_number & 0x07) << (channel * 3));
}

// Transmits 24 bits (3 bytes) of configuration information and
// reads back 24 bytes of data (3 bytes/ 24 bits for each channel)
// 24 bits: 18 bit data + 3 bit config + 3 bit channel number
// Read back is done in a new cycle
void LTC23XX_read(uint8_t cs_pin, uint32_t config_word, uint8_t data_array[24])
{
  int i, j;
  uint8_t tx_array[24];

  tx_array[23] = (uint8_t)(config_word >> 16);
  tx_array[22] = (uint8_t)(config_word >> 8);
  tx_array[21] = (uint8_t)(config_word);
  for (i = 20; i >= 0; --i)
  {
    tx_array[i] = 0;
  }
  spi_transfer_block(cs_pin, tx_array, data_array, 24);
}

int32_t sign_extend_17(uint32_t data)
{
  uint8_t sign;
  uint32_t mask = 0x20000;
  int32_t data_signed = data;
  sign = (data & mask) >> 17;
  if (sign)
    data_signed = data_signed | 0xFFFC0000;
  return data_signed;
}

// Calculates the voltage from ADC output data depending on the channel configuration
float LTC23XX_voltage_calculator(uint32_t data, uint8_t channel_configuration)
{
  float voltage;
  int32_t data_signed;
  switch (channel_configuration)
  {
    case 0:
      voltage = 0;
      break;   // Disable Channel
    case 1:
      voltage = (float)data * (1.25 * VREF / 1.000) / POW2_18;
      break;
    case 2:
      data_signed = sign_extend_17(data);
      voltage = (float)data_signed * (1.25 * VREF / 1.024) / POW2_17;
      break;
    case 3:
      data_signed = sign_extend_17(data);
      voltage = (float)data_signed * (1.25 * VREF / 1.000) / POW2_17;
      break;
    case 4:
      voltage = (float)data * (2.50 * VREF / 1.024) / POW2_18;
      break;
    case 5:
      voltage = (float)data * (2.50 * VREF / 1.000) / POW2_18;
      break;
    case 6:
      data_signed = sign_extend_17(data);
      voltage = (float)data_signed * (2.50 * VREF / 1.024) / POW2_17;
      break;
    case 7:
      data_signed = sign_extend_17(data);
      voltage = (float)data_signed * (2.50 * VREF ) / POW2_17;
      break;
  }
  return voltage;
}

