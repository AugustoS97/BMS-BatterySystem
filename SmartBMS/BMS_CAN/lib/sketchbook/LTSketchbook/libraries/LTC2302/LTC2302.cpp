/*!
LTC2302: 12-Bit, 1-Channel 500ksps SAR ADC with SPI Interface.

@verbatim

The LTC2302 is a single-channel 12-bit A/D converter with with serial I/O.
The part can be configured to operate on unipolar or bipolar modes. The
automatic sleep mode benefits power sensitive applications.

SPI DATA FORMAT (MSB First):

            Byte #1                           Byte #2
Data In  :   X  OS   X   X  UNI  X    X   X    X   X   X   X   X   X   X   X
Data Out :  D15 D14 D13 D12 D11 D10  D9  D8   D7  D6  D5  D4  D3  D2  D1  D0

OS   : ODD/Sign Bit
UNI  : Unipolar/Bipolar Bit
Dx   : Data Bits
X    : Don't care

@endverbatim

http://www.linear.com/product/LTC2302

http://www.linear.com/product/LTC2302#demoboards


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
//! @defgroup LTC2302 LTC2302: 12-Bit, 1-Channel 500ksps SAR ADC with SPI Interface.
//! @}

/*! @file
    @ingroup LTC2302
    Library for LTC2302: 12-Bit, 1-Channel 500ksps SAR ADC with SPI Interface.
*/

#include "LT_SPI.h"
#include <SPI.h>
#include "LTC2302.h"

// Reads the ADC  and returns 16-bit data
void LTC2302_read(uint8_t cs, uint16_t adc_command, uint16_t *adc_code)
{
  spi_transfer_word(cs, (uint16_t)(adc_command), adc_code);
  spi_transfer_word(cs, (uint16_t)(adc_command), adc_code); // Throws out last reading and starts a new connection
}

// Calculates the LTC2302 input voltage given the data, range, and unipolar/bipolar status.
float LTC2302_code_to_voltage(uint16_t adc_code, float vref, uint8_t uni_bipolar)
{
  float voltage;
  float sign = 1;

  if (uni_bipolar == LTC2302_UNIPOLAR)
  {
    voltage = (float)adc_code;
    voltage = vref * voltage / (4095);          //! This calculates the actual voltage at the input (in volts)
  }
  else
  {
    vref = vref/2;
    if ((adc_code & 0x8000) == 0x8000)  //adc code is < 0
    {
      adc_code = (adc_code ^ 0xFFFF)+1;    //! Convert ADC code from two's complement to binary
      sign = -1;
    }
    voltage = sign*(float)adc_code;
    voltage = vref * voltage / 2047;           //! This calculates the actual voltage at the input (in volts)
  }

  return(voltage);
}
