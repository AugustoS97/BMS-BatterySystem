/***************************************************************************//**
 *   @file   platform_drivers.h
 *   @brief  Header file of Generic Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef PLATFORM_DRIVERS_H_
#define PLATFORM_DRIVERS_H_

// Our platform driver needs to be C-compatible to work with the drivers
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SUCCESS		0
#define FAILURE		-1

#define	SPI_CPHA	0x01
#define	SPI_CPOL	0x02

#define GPIO_OUT	0x01
#define GPIO_IN		0x00

#define GPIO_HIGH	0x01
#define GPIO_LOW	0x00

#define MUX_I2C		0
#define MUX_SPI		1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum i2c_type{
	GENERIC_I2C
} i2c_type;

typedef struct i2c_init_param{
	i2c_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t		slave_address;
} i2c_init_param;

typedef struct i2c_desc{
	i2c_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t		slave_address;
} i2c_desc;

typedef enum spi_type{
	GENERIC_SPI
} spi_type;

// typedef enum {
	// SPI_MODE_0 = (0 | 0),
	// SPI_MODE_1 = (0 | SPI_CPHA),
	// SPI_MODE_2 = (SPI_CPOL | 0),
	// SPI_MODE_3 = (SPI_CPOL | SPI_CPHA)
// } spi_mode;



typedef struct spi_init_param{
	spi_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t mode;  // spi_mode	mode;
	uint8_t		chip_select;
} spi_init_param;

typedef struct {
	spi_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t mode; // spi_mode	mode;
	uint8_t		chip_select;
} spi_desc;

typedef enum {
	GENERIC_GPIO
} gpio_type;

typedef struct {
	gpio_type	type;
	uint32_t	id;
	uint8_t		number;
} gpio_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the I2C communication peripheral. */
int32_t i2c_init(i2c_desc **desc,
		 const struct i2c_init_param *param);

/* Free the resources allocated by i2c_init(). */
int32_t i2c_remove(i2c_desc *desc);

/* Write data to a slave device. */
int32_t i2c_write(i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit);

/* Read data from a slave device. */
int32_t i2c_read(i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit);

/* Initialize the SPI communication peripheral. */
int32_t spi_init(spi_desc **desc,
		 const struct spi_init_param *param);

/* Free the resources allocated by spi_init() */
int32_t spi_remove(spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t spi_write_and_read(spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number);

/* Obtain the GPIO decriptor. */
int32_t gpio_get(gpio_desc **desc,
		 uint8_t gpio_number);

/* Free the resources allocated by gpio_get() */
int32_t gpio_remove(gpio_desc *desc);

/* Enable the input direction of the specified GPIO. */
int32_t gpio_direction_input(gpio_desc *desc);

/* Enable the output direction of the specified GPIO. */
int32_t gpio_direction_output(gpio_desc *desc,
			      uint8_t value);

/* Get the direction of the specified GPIO. */
int32_t gpio_get_direction(gpio_desc *desc,
			   uint8_t *direction);

/* Set the value of the specified GPIO. */
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value);

/* Get the value of the specified GPIO. */
int32_t gpio_get_value(gpio_desc *desc,
		       uint8_t *value);

/* Generate miliseconds delay. */
void mdelay(uint32_t msecs);

/*************************
 * Linduino I2C functions for ADI use
 * These functions are wrappers around the Arduino Wire library
**/

void quikeval_set_mux(uint8_t mux);

/*! Connects and initializes I2C */
void Wire_Connect();

/*! Write I2C */
uint8_t Wire_Write(unsigned char address, unsigned char* data, unsigned char length, unsigned char stop);

/*! Read I2C */
uint8_t Wire_Read(unsigned char address, unsigned char* data, unsigned char length, unsigned char stop);

/*************************
 * Linduino SPI functions for ADI use:
 * These functions are derived from functions in LT_SPI and modified
 * to work on ADI parts. They are copied here so we don't have to
 * depend on the LT_SPI library to avoid naming conflicts.
 */
 
/*! Enable SPI, matches spi_enable */
//void Lin_SPI_Enable(uint8_t spi_clock_divider);

/*! Initialize SPI, matches spi_init */
//void Lin_SPI_Init();

/*! Connect to SPI, matches quikeval_spi_connect */
//void Lin_SPI_Connect();

void uartTX(char *buf);

#ifdef __cplusplus // Closing extern c
}
#endif

#endif // PLATFORM_DRIVERS_H_
