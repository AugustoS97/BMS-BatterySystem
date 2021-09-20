/*!
LTC2492: 24-Bit, 4-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
@verbatim

The LTC2492 is a 4-channel (2-differential) 24-bit No Latency Delta Sigma
TM ADC with Easy Drive technology. The patented sampling scheme eliminates
dynamic input current errors and the shortcomings of on-chip buffering
through automatic cancellation of differential input current. This allows
large external source impedances, and rail-to-rail input signals to be
directly digitized while maintaining exceptional DC accuracy.

@endverbatim

http://www.linear.com/product/LTC2492

http://www.linear.com/product/LTC2492#demoboards


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
//! @defgroup LTC2492 LTC2492: 24-Bit, 4-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
//! @}

/*! @file
    @ingroup LTC2492
    Library for LTC2492: 24-Bit, 4-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2492.h"
#include "LTC24XX_general.h"

// Checks for EOC with a specified timeout.
// Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2492_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
{
  return(LTC24XX_EOC_timeout(cs, miso_timeout));
}

// Reads from LTC2492.
void LTC2492_read(uint8_t cs, uint8_t adc_command_high, uint8_t adc_command_low, int32_t *adc_code)
{
  LTC24XX_SPI_16bit_command_32bit_data(cs, adc_command_high, adc_command_low, adc_code);    // Transfer arrays
}

// Calculates the voltage corresponding to an adc code, given lsb weight (in volts)
float LTC2492_code_to_voltage(int32_t adc_code, float vref)
{
  return(LTC24XX_diff_code_to_voltage(adc_code, vref));
}
