/*!
    LTC6948: Ultralow Noise and Spurious 0.37GHz to 6.39GHz FracN Synthesizer with Integrated VCO

@verbatim

The LTC®6948 is a high performance, low noise, 6.39GHz
phase-locked loop (PLL) with a fully integrated VCO,
including a reference divider, phase-frequency detector
(PFD), ultralow noise charge pump, fractional feedback
divider, and VCO output divider.

The fractional divider uses an advanced, 4th order ΔΣ
modulator which provides exceptionally low spurious
levels. This allows wide loop bandwidths, producing extremely
low integrated phase noise values.

The programmable VCO output divider, with a range of 1
through 6, extends the output frequency range.

@endverbatim


http://www.linear.com/product/LTC6948

http://www.linear.com/product/LTC6948#demoboards


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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC6948 LTC6948: Ultralow Noise and Spurious 0.37GHz to 6.39GHz FracN Synthesizer with Integrated VCO
//! @}

/*! @file
    @ingroup LTC6948
    Library for LTC6948: Ultralow Noise and Spurious 0.37GHz to 6.39GHz FracN Synthesizer with Integrated VCO
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_SPI.h"
#include "LTC6948.h"
#include <SPI.h>

uint8_t LTC6948_reg[LTC6948_NUM_REGADDR];               //!< number of LTC6948 spi addresses
uint8_t LTC6948_spi_map[(LTC6948_NUM_REGFIELD+1)][4];   //!< LTC6948 spi map, stores MSB address location, MSB bit location, field length in bits, and R or RW capability

unsigned long LTC6948_Fref_MHz = 100;                   //!< Default Fref frequency - MHz portion (xxx);  Fref = xxx, yyy,yyy
unsigned long LTC6948_Fref_Hz = 0;                      //!< Default Fref frequency - Hz portion (yyy,yyy);  Fref = x,xxx, yyy,yyy
unsigned long LTC6948_Frf_MHz = 907;                    //!< Default Frf frequency - MHz portion (xxxx);  Frf = x,xxx, yyy,yyy
unsigned long LTC6948_Frf_Hz  = 0;                      //!< Default Frf frequency - Hz portion (yyy,yyy);  Frf = x,xxx, yyy,yyy

unsigned long zero64[]= {0,0}; //!< for 64bit math functions

/* -------------------------------------------------------------------------
  FUNCTION: LTC6948_read
  - reads 8 bit Data field to LTC6948.
  - has to shift data by one bit to account for RW bit
 -------------------------------------------------------------------------- */
uint8_t LTC6948_read(uint8_t cs, int8_t address)
{
  int8_t address_shift;
  LT_union_int16_2bytes rx;

  address_shift =(address << 1) | 0x01; // shift to left to account for R/W bit, set bit high for read
  spi_transfer_word(cs, address_shift<<8 , &rx.LT_uint16);

  LTC6948_reg[address]=rx.LT_byte[0];
  return(rx.LT_byte[0]);
}


/* -------------------------------------------------------------------------
  FUNCTION: LTC6948_read_field
  For SPI FIELDS located in 1 or multiple address location
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long LTC6948_read_field(uint8_t cs, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
{
  int bit_shift, i, num_reg;
  long field_val, maskbits, pow2;

  num_reg=0;
  field_val=0;
// determines how many register are used
  do
  {
    bit_shift = (MSB_loc+1)- (numbits-num_reg*8);   // determines bit_shift for last register location
    field_val=LTC6948_read(cs, (address+num_reg))+(field_val<<8);  // reads current address locations, shifts previous address location 8 bits
    num_reg++;
  }
  while ((bit_shift<0) && (num_reg<4));

// creates a bit mask for complete word,
  maskbits = 1;
  pow2=1;
  for (i=1, maskbits=1; i<numbits; i++)
  {
    pow2=pow2*2;
    maskbits = maskbits+pow2;
  }

  field_val=(field_val >>bit_shift) &maskbits;
  return field_val;
}

/* -------------------------------------------------------------------------
  FUNCTION: get_LTC6948_SPI_FIELD
  For SPI FIELDS
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long get_LTC6948_SPI_FIELD(uint8_t cs, uint8_t f)
{

  return LTC6948_read_field(cs, LTC6948_spi_map[f][ADDRx], LTC6948_spi_map[f][DxMSB], LTC6948_spi_map[f][NUMBITS]);
}

/* -------------------------------------------------------------------------
  FUNCTION: LTC6948_write
  - writes 8 bit Data field to LTC6948.
  - has to shift data by one bit to account for RW bit
--------------------------------------------------------------------------- */
void LTC6948_write(uint8_t cs, uint8_t address, uint8_t Data)
{
  LT_union_int16_2bytes rx;

  address=address << 1; // shift to left to account for R/W bit
  spi_transfer_word(cs, (address<<8) | Data, &rx.LT_uint16);
}


/* -------------------------------------------------------------------------
 FUNCTION: LTC6948_write_field
 For SPI FIELDS
 - reads specific address location
 - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
uint8_t LTC6948_write_field(uint8_t cs, long field_data, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
{
  long current_content, desired_content, reg_val;
  int LSB_loc, i, j, num_reg, bit_shift;
  long temp_arr[32];

  for (i=0; i<32 ; i++) temp_arr[i]=0;  // init temp_arr

// read data in current address location and put in a bit array
  num_reg=0;
  current_content=0;
  do
  {
    bit_shift=(MSB_loc+1)-(numbits-num_reg*8);
    current_content=LTC6948_read(cs, (address+num_reg)) + (current_content<<8);

    num_reg++;
  }
  while ((bit_shift<0) && (num_reg<4));
  for (i=0; i<(8*num_reg); i++)
  {
    temp_arr[i]=(current_content>>i) & 1;
  }

// exchange current bits with desired bits
  LSB_loc = 8*(num_reg-1)+MSB_loc-numbits+1;
  for (i=LSB_loc, j=0; i<=(MSB_loc+(num_reg-1)*8); i++, j++)
  {
    temp_arr[i] = (field_data>>j) &1;
  } // end of for loop

// reconstruct bits into an integer
  desired_content = 0;
  for (i=0; i<(8*num_reg); i++)
  {
    desired_content = desired_content | (temp_arr[i]<<i);
  } // end of for loop

// write new field value to part
  for (i=0; i<num_reg; i++)
  {
    reg_val = (desired_content >> 8*(num_reg-1-i)) & 0xff;
    LTC6948_write(cs, (address+i), reg_val);
  } // end of for loop
} // end of LTC6948_write_field


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6948_REGSIZE
   - returns # of addresses in parts register map (array size)
---------------------------------------------------------------------------- */
uint8_t get_LTC6948_REGSIZE()
{
  return sizeof(LTC6948_reg);
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6948_SPI_FIELD_NUMBITS
   - returns the number of bits for a given field name in the SPI map
---------------------------------------------------------------------------- */
uint8_t get_LTC6948_SPI_FIELD_NUMBITS(uint8_t f)
{
  return LTC6948_spi_map[f][NUMBITS];
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6948_SPI_FIELD_RW
   - returns if the given field name is (0)read/write or (1)read_only field
---------------------------------------------------------------------------- */
uint8_t get_LTC6948_SPI_FIELD_RW(uint8_t f)
{
  return LTC6948_spi_map[f][R_ONLY];
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6948_SPI_FIELD
   For SPI FIELDS
   - reads specific address location
   - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
void set_LTC6948_SPI_FIELD(uint8_t cs, uint8_t f, long field_data)
{
  LTC6948_write_field(cs, field_data, LTC6948_spi_map[f][ADDRx], LTC6948_spi_map[f][DxMSB], LTC6948_spi_map[f][NUMBITS]);
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6948_ALLREGS
   - writes data to all registers at once
--------------------------------------------------------------------------- */
void set_LTC6948_ALLREGS(uint8_t cs, uint8_t reg01, uint8_t reg02, uint8_t reg03, uint8_t reg04, uint8_t reg05, uint8_t reg06, uint8_t reg07, uint8_t reg08, uint8_t reg09, uint8_t reg0A, uint8_t reg0B, uint8_t reg0C, uint8_t reg0D)
{
  uint8_t i;

  LTC6948_reg[1] = reg01;
  LTC6948_reg[2] = reg02;
  LTC6948_reg[3] = reg03;
  LTC6948_reg[4] = reg04;
  LTC6948_reg[5] = reg05;
  LTC6948_reg[6] = reg06;
  LTC6948_reg[7] = reg07;
  LTC6948_reg[8] = reg08;
  LTC6948_reg[9] = reg09;
  LTC6948_reg[10] = reg0A;
  LTC6948_reg[11] = reg0B;
  LTC6948_reg[12] = reg0C;
  LTC6948_reg[13] = reg0D;

  for (i=1; i<14; i++)  LTC6948_write(cs, i, LTC6948_reg[i]);
} // end of set_LTC6948_ALLREGS


/* -------------------------------------------------------------------------
   FUNCTION: LTC6948_init
   - initializes the SPI MAP
   - for ease of programming there is spreadsheet that automates this some.
----------------------------------------------------------------------------*/
void LTC6948_init()
{

// spi map
  LTC6948_spi_map[LTC6948_ALCCAL][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_ALCCAL][DxMSB]= 5;
  LTC6948_spi_map[LTC6948_ALCCAL][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_ALCEN][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_ALCEN][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_ALCEN][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_ALCHI][ADDRx]=0x00;
  LTC6948_spi_map[LTC6948_ALCHI][DxMSB]= 4;
  LTC6948_spi_map[LTC6948_ALCHI][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_ALCLO][ADDRx]=0x00;
  LTC6948_spi_map[LTC6948_ALCLO][DxMSB]= 3;
  LTC6948_spi_map[LTC6948_ALCLO][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_ALCMON][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_ALCMON][DxMSB]= 6;
  LTC6948_spi_map[LTC6948_ALCMON][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_ALCULOK][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_ALCULOK][DxMSB]= 4;
  LTC6948_spi_map[LTC6948_ALCULOK][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_AUTOCAL][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_AUTOCAL][DxMSB]= 3;
  LTC6948_spi_map[LTC6948_AUTOCAL][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_AUTORST][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_AUTORST][DxMSB]= 2;
  LTC6948_spi_map[LTC6948_AUTORST][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_BD][ADDRx]=0x04;
  LTC6948_spi_map[LTC6948_BD][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_BD][NUMBITS]= 4;
  LTC6948_spi_map[LTC6948_BST][ADDRx]=0x0b;
  LTC6948_spi_map[LTC6948_BST][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_BST][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CAL][ADDRx]=0x0a;
  LTC6948_spi_map[LTC6948_CAL][DxMSB]= 0;
  LTC6948_spi_map[LTC6948_CAL][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CP][ADDRx]=0x0c;
  LTC6948_spi_map[LTC6948_CP][DxMSB]= 2;
  LTC6948_spi_map[LTC6948_CP][NUMBITS]= 3;
  LTC6948_spi_map[LTC6948_CPCHI][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPCHI][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_CPCHI][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPCLO][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPCLO][DxMSB]= 6;
  LTC6948_spi_map[LTC6948_CPCLO][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPDN][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPDN][DxMSB]= 0;
  LTC6948_spi_map[LTC6948_CPDN][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPINV][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPINV][DxMSB]= 4;
  LTC6948_spi_map[LTC6948_CPINV][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPLE][ADDRx]=0x04;
  LTC6948_spi_map[LTC6948_CPLE][DxMSB]= 3;
  LTC6948_spi_map[LTC6948_CPLE][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPMID][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPMID][DxMSB]= 5;
  LTC6948_spi_map[LTC6948_CPMID][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPRST][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPRST][DxMSB]= 2;
  LTC6948_spi_map[LTC6948_CPRST][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPUP][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPUP][DxMSB]= 1;
  LTC6948_spi_map[LTC6948_CPUP][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_CPWIDE][ADDRx]=0x0d;
  LTC6948_spi_map[LTC6948_CPWIDE][DxMSB]= 3;
  LTC6948_spi_map[LTC6948_CPWIDE][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_DITHEN][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_DITHEN][DxMSB]= 1;
  LTC6948_spi_map[LTC6948_DITHEN][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_FILT][ADDRx]=0x0b;
  LTC6948_spi_map[LTC6948_FILT][DxMSB]= 6;
  LTC6948_spi_map[LTC6948_FILT][NUMBITS]= 2;
  LTC6948_spi_map[LTC6948_INTN][ADDRx]=0x03;
  LTC6948_spi_map[LTC6948_INTN][DxMSB]= 0;
  LTC6948_spi_map[LTC6948_INTN][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_LDOEN][ADDRx]=0x04;
  LTC6948_spi_map[LTC6948_LDOEN][DxMSB]= 2;
  LTC6948_spi_map[LTC6948_LDOEN][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_LDOV][ADDRx]=0x04;
  LTC6948_spi_map[LTC6948_LDOV][DxMSB]= 1;
  LTC6948_spi_map[LTC6948_LDOV][NUMBITS]= 2;
  LTC6948_spi_map[LTC6948_LKCT][ADDRx]=0x0c;
  LTC6948_spi_map[LTC6948_LKCT][DxMSB]= 4;
  LTC6948_spi_map[LTC6948_LKCT][NUMBITS]= 2;
  LTC6948_spi_map[LTC6948_LKWIN][ADDRx]=0x0c;
  LTC6948_spi_map[LTC6948_LKWIN][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_LKWIN][NUMBITS]= 3;
  LTC6948_spi_map[LTC6948_LOCK][ADDRx]=0x00;
  LTC6948_spi_map[LTC6948_LOCK][DxMSB]= 2;
  LTC6948_spi_map[LTC6948_LOCK][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_MTCAL][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_MTCAL][DxMSB]= 2;
  LTC6948_spi_map[LTC6948_MTCAL][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_ND][ADDRx]=0x06;
  LTC6948_spi_map[LTC6948_ND][DxMSB]= 1;
  LTC6948_spi_map[LTC6948_ND][NUMBITS]= 10;
  LTC6948_spi_map[LTC6948_NUM][ADDRx]=0x08;
  LTC6948_spi_map[LTC6948_NUM][DxMSB]= 5;
  LTC6948_spi_map[LTC6948_NUM][NUMBITS]= 18;
  LTC6948_spi_map[LTC6948_OD][ADDRx]=0x0b;
  LTC6948_spi_map[LTC6948_OD][DxMSB]= 2;
  LTC6948_spi_map[LTC6948_OD][NUMBITS]= 3;
  LTC6948_spi_map[LTC6948_OMUTE][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_OMUTE][DxMSB]= 1;
  LTC6948_spi_map[LTC6948_OMUTE][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_PART][ADDRx]=0x0e;
  LTC6948_spi_map[LTC6948_PART][DxMSB]= 3;
  LTC6948_spi_map[LTC6948_PART][NUMBITS]= 4;
  LTC6948_spi_map[LTC6948_PDALL][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_PDALL][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_PDALL][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_PDFN][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_PDFN][DxMSB]= 3;
  LTC6948_spi_map[LTC6948_PDFN][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_PDOUT][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_PDOUT][DxMSB]= 4;
  LTC6948_spi_map[LTC6948_PDOUT][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_PDPLL][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_PDPLL][DxMSB]= 6;
  LTC6948_spi_map[LTC6948_PDPLL][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_PDVCO][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_PDVCO][DxMSB]= 5;
  LTC6948_spi_map[LTC6948_PDVCO][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_POR][ADDRx]=0x02;
  LTC6948_spi_map[LTC6948_POR][DxMSB]= 0;
  LTC6948_spi_map[LTC6948_POR][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_RD][ADDRx]=0x06;
  LTC6948_spi_map[LTC6948_RD][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_RD][NUMBITS]= 5;
  LTC6948_spi_map[LTC6948_REV][ADDRx]=0x0e;
  LTC6948_spi_map[LTC6948_REV][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_REV][NUMBITS]= 4;
  LTC6948_spi_map[LTC6948_RFO][ADDRx]=0x0b;
  LTC6948_spi_map[LTC6948_RFO][DxMSB]= 4;
  LTC6948_spi_map[LTC6948_RFO][NUMBITS]= 2;
  LTC6948_spi_map[LTC6948_RSTFN][ADDRx]=0x0a;
  LTC6948_spi_map[LTC6948_RSTFN][DxMSB]= 1;
  LTC6948_spi_map[LTC6948_RSTFN][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_SEED][ADDRx]=0x05;
  LTC6948_spi_map[LTC6948_SEED][DxMSB]= 7;
  LTC6948_spi_map[LTC6948_SEED][NUMBITS]= 8;
  LTC6948_spi_map[LTC6948_THI][ADDRx]=0x00;
  LTC6948_spi_map[LTC6948_THI][DxMSB]= 1;
  LTC6948_spi_map[LTC6948_THI][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_TLO][ADDRx]=0x00;
  LTC6948_spi_map[LTC6948_TLO][DxMSB]= 0;
  LTC6948_spi_map[LTC6948_TLO][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_UNLOK][ADDRx]=0x00;
  LTC6948_spi_map[LTC6948_UNLOK][DxMSB]= 5;
  LTC6948_spi_map[LTC6948_UNLOK][NUMBITS]= 1;
  LTC6948_spi_map[LTC6948_x][ADDRx]=0x01;
  LTC6948_spi_map[LTC6948_x][DxMSB]= 5;
  LTC6948_spi_map[LTC6948_x][NUMBITS]= 6;

  LTC6948_spi_map[LTC6948_ALCCAL][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_ALCEN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_ALCHI][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_ALCLO][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_ALCMON][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_ALCULOK][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_AUTOCAL][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_AUTORST][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_BD][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_BST][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CAL][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CP][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPCHI][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPCLO][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPDN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPINV][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPLE][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPMID][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPRST][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPUP][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_CPWIDE][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_DITHEN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_FILT][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_INTN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_LDOEN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_LDOV][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_LKCT][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_LKWIN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_LOCK][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_MTCAL][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_ND][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_NUM][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_OD][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_OMUTE][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_PART][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_PDALL][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_PDFN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_PDOUT][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_PDPLL][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_PDVCO][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_POR][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_RD][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_REV][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_RFO][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_RSTFN][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_SEED][R_ONLY]= 0;
  LTC6948_spi_map[LTC6948_THI][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_TLO][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_UNLOK][R_ONLY]= 1;
  LTC6948_spi_map[LTC6948_x][R_ONLY]= 0;

} // end of LTC6948_init

void set_LTC6948_global_fref(unsigned long fref_MHz, unsigned long fref_Hz)
{
  LTC6948_Fref_MHz=fref_MHz;
  LTC6948_Fref_Hz=fref_Hz;
}

void set_LTC6948_global_frf(unsigned long frf_MHz, unsigned long frf_Hz)
{
  LTC6948_Frf_MHz=frf_MHz;
  LTC6948_Frf_Hz=frf_Hz;
}

unsigned long get_LTC6948_global_fref_MHz()
{
  return LTC6948_Fref_MHz;
}

unsigned long get_LTC6948_global_fref_Hz()
{
  return LTC6948_Fref_Hz;
}

unsigned long get_LTC6948_global_frf_MHz()
{
  return LTC6948_Frf_MHz;
}

unsigned long get_LTC6948_global_frf_Hz()
{
  return LTC6948_Frf_Hz;
}


/* -------------------------------------------------------------------------
   FUNCTION: calc_odiv
   - calculates the output divider setting based on the frf and version of LTC6948
   - @return odiv = 1-6 for valid setting, 999 as invalid frequency
---------------------------------------------------------------------------- */
unsigned long LTC6948_calc_odiv(char part_version[], unsigned long frf[2])
{
  unsigned long odiv, max_fout, min_fout, temp_val,i, temp_out;
  unsigned long max_fout64[2];
  unsigned long min_fout64[2];
  unsigned long temp_fout[2];
  unsigned long temp_i[2];
  boolean valid_input=false;

// Determine which frequency range to verify too based on part version
  if (part_version[8]=='1')    // if this is a LTC6948-1
  {
    max_fout=LTC6948_1_MAXFREQ;
    min_fout=LTC6948_1_MINFREQ;
  }
  else if (part_version[8]=='2')   // if this is a LTC6948-2
  {
    max_fout=LTC6948_2_MAXFREQ;
    min_fout=LTC6948_2_MINFREQ;
  }
  else if (part_version[8]=='3')   // if this is a LTC6948-3
  {
    max_fout=LTC6948_3_MAXFREQ;
    min_fout=LTC6948_3_MINFREQ;
  }
  else if (part_version[8]=='4')   // if this is a LTC6948-4
  {
    max_fout=LTC6948_4_MAXFREQ;
    min_fout=LTC6948_4_MINFREQ;
  }
  else
  {
    Serial.print("No default Frequencies for this board:  ");
    Serial.println(part_version);
  }  // end if-then-else statement
  HZto64(max_fout64,max_fout,0L);
  HZto64(min_fout64,min_fout,0L);

// verify desired frequency falls within a divider range (1-6)
  valid_input=false;
  for (i=1; i<=6; i++)
  {
    init64(temp_i,0L,i);
    temp_fout[0] = frf[0];
    temp_fout[1] = frf[1];
    mul64(temp_fout,temp_i);
    if (lt64(temp_fout,max_fout64) || eq64(temp_fout, max_fout64))    // same as frf*i <= max_fout
    {
      if (lt64(min_fout64,temp_fout) || eq64(temp_fout, min_fout64))  // same as frf*i >= min_fout
      {
        valid_input=true;
        odiv=i;
      }
    }
  } // end of for loop

  if (valid_input == false) odiv= 999L;
  return odiv;

} // end of LTC6948_calc_odiv

/* -------------------------------------------------------------------------
   FUNCTION: LTC6948_set_frf
   Calculates the integer (N), fractional (NUM) and output divider (OD) SPI values
   using self created 64bit math functions.

  Datasheet equations
    fvco = fpfd*(N + F)
    frf  = fvco/O
    fpfd = fref/R
    F=NUM/2^18 = NUM/262144

    can be modified to the following equations
    N   = (int) (fvco/fpfd)  = (int) frf*O*R/fref
    NUM = (int) [262144 * (frf*O*R/fref -N)]

    where
    N = ndiv, O= odiv, NUM=fracN_num  in the code below

    Linduino One (Arduino Uno) is limited to 32 bit floats/double/long.
    32 bit math functions will create rounding errors with the above equations,
    that can result in frequency errors.
    Therefore, the following code uses self created 64bit functions for 64bit integer math.

    frf (33 bits) LTC6948-4 max frf/fvco = 6.4GHZ, which is 23 bit number (2^33 = 8.59G)
    fref (23 bits) LTC6948 min fref = 10MHz, which is a 23 bit number (2^23 = 8.3M)
    O   (3 bits)
    R   (5 bits)

    step 1: create 64 bit frf and fref numbers

    step 2: calculate O (output divider)

    step 3: get current R-divider setting

    step 4: calculate frf*O*R
               max bit count/resolution: 33b+3b+5b= 41b
    step 5: calculate N(10b), using value from step 1
               N = (int) frf*O*R/fref
               max bit count/resolution: 41b-23b = 18b
    step 6: calculate NUM (18b)
               NUM = (int) [262144 * (frf*O*R/fref -N)]
                   = (int) [2^18   * (frf*O*R/fref -N)]
                to get the correct value the above calculation are performed like this
                   =  ((int)((frf*O*R/fref)<<23) - N<<23 + rounding) >> 5,  where << & >> are  bit shift left, right
                      * note << 23b was chosen, because step one use 41b,  41b +23b = 64b (max number for 64 bit math)
                      ** note >> 5 was chosen because 23-5 = 2^18, which is the same as multiplying by 262144
                      *** the rounding variable is equivalent to adding 0.5 to the NUM number.  The integer operation always
                          rounds down, so adding a 0.5 to the number before rounding down ensures the closest value is
                          selected.
                          In this function the lower 5 bits of  (frf*O*R/fref)<<23) - N<<23 (possible variable 0-31)
                          are check to see if they are >=16.
                          If they are then 16 is added to this number before performing the int and >>5 functions.
---------------------------------------------------------------------------- */
void LTC6948_set_frf(char part_version[])
{
  unsigned long frf_MHz, frf_Hz, fref_MHz, fref_Hz, odiv, rdiv, ndiv, fracN_num, frac_num_remainder;
  unsigned long N64[2], NUM64[2], R64[2], O64[2], temp_long[2];
  char buffer[100];
  unsigned long frf[2], frf_act[2];
  unsigned long fref[2];
  unsigned long temp_math[2];
  unsigned long frf_rdiv_odiv[2];
  unsigned long frf_rdiv_odiv_int[2];
  unsigned long roundup[2];

  /* step 1: create 64 bit frf and fref numbers
     32 bit xxxx MHz number and 32 bit yyy yyy Hz number. */
  frf_MHz=LTC6948_Frf_MHz;
  frf_Hz=LTC6948_Frf_Hz;
  HZto64(frf, frf_MHz, frf_Hz);

  fref_MHz=LTC6948_Fref_MHz;
  fref_Hz=LTC6948_Fref_Hz;
  HZto64(fref,fref_MHz,fref_Hz);

// step 2: calculate O (output divider)
  odiv=LTC6948_calc_odiv(part_version,frf);

// step 3: get current R-divider setting
  rdiv=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_RD);    // reads selected field

// step 3: calculate frf*O*R
  frf_rdiv_odiv[0]=0;
  frf_rdiv_odiv[1]=odiv*rdiv;
  mul64(frf_rdiv_odiv, frf);  // odiv*rdiv*frf
  frf_rdiv_odiv_int[0]=frf_rdiv_odiv[0];  // copy odiv*rdiv*frf to another variable
  frf_rdiv_odiv_int[1]=frf_rdiv_odiv[1];

// step 4: calculate N(10b), using value from step 3; N = (int) frf*O*R/fref
  temp_math[0]=fref[0];   // copy fref to temp variable for math operation
  temp_math[1]=fref[1];
  div64(frf_rdiv_odiv_int, temp_math);   // frf_rdiv_odiv_int= [(frf*odiv*rdiv)]/fref  -->  int(fvco/fpfd)
  ndiv=frf_rdiv_odiv_int[1];

// step 5: calculate NUM (18b), NUM = (int) [262144 * (frf*O*R/fref -N)]
// SEE notes in function header for step 5
// ((int)((frf*O*R/fref)<<23) - N<<23 + rounding) >> 5,  where << & >> are  bit shift left, right
  shl64by(frf_rdiv_odiv,23);
  div64(frf_rdiv_odiv,  fref);    // frf_rdiv_odiv     -->  [(frf*odiv*rdiv)<<23]/fref

  shl64by(frf_rdiv_odiv_int,23); // frf_rdiv_odiv_int  -->  [(int)((frf*odiv*rdiv)/fref)] <<23

  sub64(frf_rdiv_odiv,frf_rdiv_odiv_int);

// NUM is a 18 bit number in the spi register
// At this point in the program it is represented by 23 bits
// if the lower 5 bits are >=16 (16/31),
// then add 16 to the frac_num_remainder.  After the bits are
// shifted down 5 bits this will be close to added 0.5 bits.
  frac_num_remainder=(frf_rdiv_odiv[1] & 31L);
  if (frac_num_remainder >= 16L)
  {
    roundup[0]=0L;
    roundup[1]=16L;
    add64(frf_rdiv_odiv,roundup);
  }

// shift NUM to get desired 18 bit number for SPI map.
  shr64by(frf_rdiv_odiv,5);
  fracN_num=frf_rdiv_odiv[1];

// program part and print out results to screen
  set_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_OD,odiv);    // programs output divider
  Serial.print("OD = ");
  Serial.println(odiv);

  Serial.print("RD = ");
  Serial.println(rdiv);

  set_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ND,ndiv);    // programs N-divider
  Serial.print(F("ND = "));
  Serial.println(ndiv);

  if (fracN_num > 0L)
  {
    set_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_INTN,0L);    // programs fractional mode
    set_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_NUM,fracN_num);    // programs NUM
    if ( (ndiv<35) || (ndiv>1019) ) Serial.print(F("N DIV set to invalid setting - REFER TO DATASHEET\n"));
  }
  else
  {
    set_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_INTN,1L);    // programs INTN
    Serial.print(F("When NUM=0, changes to Integer Mode (NUM can not be 0)\n"));
    if ( (ndiv<32) || (ndiv>1023) ) Serial.print(F("N DIV set to invalid setting - REFER TO DATASHEET\n"));
  }
  Serial.print(F("NUM = "));
  Serial.println(fracN_num);

  set_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CAL,1);    // calibrates part


/////  calculate actual fout here and print out
  frf_act[0]=fref[0];
  frf_act[1]=fref[1];
  N64[0]=0;
  NUM64[0]=0;
  R64[0]=0;
  O64[0]=0;
  N64[1]=ndiv;
  NUM64[1]=fracN_num;
  R64[1]=rdiv;
  O64[1]=odiv;

  shl64by(N64,18);
  add64(N64,NUM64);    // (N*262144 +NUM)
  div64(frf_act,R64);  // fref/rdiv (or fpfd)
  mul64(frf_act,N64);  // (N*262144 +NUM)*fpdf
  div64(frf_act,O64);  // ((N*262144 +NUM)*fpdf)/odiv
  shr64by(frf_act,17);
  if (frf_act[0]==0)   // no 32 bit overflow case
  {
    Serial.print(F("Actual Output Frequency (closest available) is "));
    Serial.print(frf_act[1]);
    Serial.println("Hz");
  }
  else    // overflow case
  {
    temp_math[0]=frf_act[0];
    temp_math[1]=frf_act[1];
    temp_long[0]=0;
    temp_long[1]=OneMHz;
    div64(temp_math,temp_long);  // frf/1e6
    Serial.print(F("Actual Output Frequency (closest available) is "));
    Serial.print(temp_math[1]);
    Serial.print("MHz + ");
    temp_long[0]=0;
    temp_long[1]=OneMHz;
    mul64(temp_math,temp_long);  // int(frf/1e6)*1e6
    sub64(frf_act,temp_math);    //  frf-int(frf/1e6)*1e6
    Serial.print(frf_act[1]);
    Serial.println("Hz");
  }
}


/* -------------------------------------------------------------------------
   FUNCTION: prt
   Prints HEX representation of 64 bit an
---------------------------------------------------------------------------- */
void prt(unsigned long  an[])
{
  Serial.print(an[0],HEX);
  Serial.print(" ");
  Serial.println(an[1],HEX);
}


/* -------------------------------------------------------------------------
   FUNCTION: init64
   Creates a equivalent 64 bit number from 2 32 bit numbers
    an[0]=bigPart    //upper 32 bits
    an[1]=littlePart //lower 32 bits
---------------------------------------------------------------------------- */
void init64(unsigned long  an[], unsigned long bigPart, unsigned long littlePart )
{
  an[0]=bigPart;
  an[1]=littlePart;
}

/* -------------------------------------------------------------------------
   FUNCTION: HZto64
   create a 64 bit Hz number from
   32 bit xxxx MHz number and 32 bit yyy yyy Hz number.
   A) if an < 2^32 bits
      an(upper 32b) = 0
      an(lower 32b) = MHzPart(32b)*1MHz + HzPart (32b)
   B) if an > 2^32 bits (4,294,967,296)
     an(upper 32b) = 1
     an(lower 32b) = ((MHzPart-4294)*1MHz+HzPart)-967296
---------------------------------------------------------------------------- */
void HZto64(unsigned long  an[], unsigned long MHzPart, unsigned long HzPart )
{

  if ((MHzPart>4295) || ((MHzPart==4294) && (HzPart>=967296)))
  {
    an[0]=1L;                                        // upper 32 bits
    an[1] =(MHzPart-4294L)*OneMHz + HzPart-967296L;  // lower 32 bits
  }
  else
  {
    an[0] = 0;                        // upper 32 bits
    an[1] = MHzPart*OneMHz+HzPart;    // lower 32 bits
  }
}

/* -------------------------------------------------------------------------
   FUNCTION: shl64
   Single Bit shift left of equivalent 64 bit number (an[] = an[]<<1)
---------------------------------------------------------------------------- */
void shl64(unsigned long  an[])
{
  an[0] <<= 1;
  if (an[1] & 0x80000000)
    an[0]++;
  an[1] <<= 1;
}


/* -------------------------------------------------------------------------
   FUNCTION: shr64
   Single Bit shift right of equivalent 64 bit number (an[] = an[]>>1)
---------------------------------------------------------------------------- */
void shr64(unsigned long  an[])
{
  an[1] >>= 1;
  if (an[0] & 0x1)
    an[1]+=0x80000000;
  an[0] >>= 1;
}


/* -------------------------------------------------------------------------
   FUNCTION: shl64by
   Multi Bit shift left of equivalent 64 bit number (an[] = an[]<<shiftnum)
---------------------------------------------------------------------------- */
void shl64by(unsigned long  an[], uint8_t shiftnum)
{
  uint8_t i;

  for (i=0; i<shiftnum; i++)
  {
    an[0] <<= 1;
    if (an[1] & 0x80000000)
      an[0]++;
    an[1] <<= 1;
  }
}


/* -------------------------------------------------------------------------
   FUNCTION: shr64by
   Multi Bit shift right of equivalent 64 bit number (an[] = an[]>>shiftnum)
---------------------------------------------------------------------------- */
void shr64by(unsigned long  an[], uint8_t shiftnum)
{
  uint8_t i;

  for (i=0; i<shiftnum; i++)
  {
    an[1] >>= 1;
    if (an[0] & 0x1)
      an[1]+=0x80000000;
    an[0] >>= 1;
  }
}


/* -------------------------------------------------------------------------
   FUNCTION: add64
   64 bit Add ann to an (an[] = an[] + ann[])
---------------------------------------------------------------------------- */
void add64(unsigned long  an[], unsigned long  ann[])
{
  an[0]+=ann[0];
  if (an[1] + ann[1] < ann[1])
    an[0]++;
  an[1]+=ann[1];
}


/* -------------------------------------------------------------------------
   FUNCTION: sub64
   64 bit Subtract ann from an (an[] = an[] - ann[])
---------------------------------------------------------------------------- */
void sub64(unsigned long  an[], unsigned long  ann[])
{
  an[0]-=ann[0];
  if (an[1] < ann[1])
  {
    an[0]--;
  }
  an[1]-= ann[1];
}


/* -------------------------------------------------------------------------
   FUNCTION: eq64
   64 bit, if an == ann, then true
---------------------------------------------------------------------------- */
boolean eq64(unsigned long  an[], unsigned long  ann[])
{
  return (an[0]==ann[0]) && (an[1]==ann[1]);
}


/* -------------------------------------------------------------------------
   FUNCTION: lt64
   64 bit, if an < ann, then true
---------------------------------------------------------------------------- */
boolean lt64(unsigned long  an[], unsigned long  ann[])
{
  if (an[0]>ann[0]) return false;
  return (an[0]<ann[0]) || (an[1]<ann[1]);
}


/* -------------------------------------------------------------------------
   FUNCTION: div64
   64 bit Divide,   num=num/div
---------------------------------------------------------------------------- */
void div64(unsigned long num[], unsigned long den[])
{
  unsigned long quot[2];
  unsigned long qbit[2];
  unsigned long tmp[2];
  init64(quot,0,0);
  init64(qbit,0,1);

  if (eq64(num, zero64))    //numerator 0, call it 0
  {
    init64(num,0,0);
    return;
  }

  if (eq64(den, zero64))   //numerator not zero, denominator 0, infinity in my book.
  {
    init64(num,0xffffffff,0xffffffff);
    return;
  }

  init64(tmp,0x80000000,0);
  while (lt64(den,tmp))
  {
    shl64(den);
    shl64(qbit);
  }

  while (!eq64(qbit,zero64))
  {
    if (lt64(den,num) || eq64(den,num))
    {
      sub64(num,den);
      add64(quot,qbit);
    }
    shr64(den);
    shr64(qbit);
  }

  //remainder now in num, but using it to return quotient for now
  init64(num,quot[0],quot[1]);
}


/* -------------------------------------------------------------------------
   FUNCTION: mul64
   64 bit multiply,   an=an*ann
---------------------------------------------------------------------------- */
void mul64(unsigned long an[], unsigned long ann[])
{
  unsigned long p[2] = {0,0};
  unsigned long y[2] = {ann[0], ann[1]};
  while (!eq64(y,zero64))
  {
    if (y[1] & 1)
      add64(p,an);
    shl64(an);
    shr64(y);
  }
  init64(an,p[0],p[1]);
}