#ifndef BMS_lib_h
#define BMS_lib_h

#include <Arduino.h>
#include <stdint.h>
//#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LTC68042.h"
#include "SPI.h"
#include "EEPROM.h"
#include "config_BMS.h"

void read_cell_voltage (const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL, uint8_t tx_cfg[][6], uint16_t cell_codes[][12]);

void init_cfg(uint8_t tx_cfg[][6], const uint8_t TOTAL_IC);

void LTC6804_initialize();

void print_config(uint8_t tx_cfg[][6], const uint8_t TOTAL_IC);

void print_cells(const uint8_t TOTAL_IC, uint16_t cell_codes[][12]);

void print_aux(uint16_t aux_codes[][6], const uint8_t TOTAL_IC);

void print_rxconfig(uint8_t rx_cfg[][8], const uint8_t TOTAL_IC);

void read_eeprom_ltc (const uint8_t TOTAL_IC, uint8_t tx_cfg[][6]);

void read_eeprom_atmega(float &UV_THR, float &OV_THR, uint8_t &N_NTC, uint8_t &TOTAL_CELL, float &UVBAT_THR, float &OVBAT_THR, float &MAX_VCELL_DIFF, uint8_t &BALANCING_TYPE, uint8_t &NCELL_PARALLEL, int16_t &CURRENT_OFFSET , uint8_t &TSLEEP);

void read_cell_voltage (const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL, uint8_t tx_cfg[][6], uint16_t cell_codes[][12]);

int32_t get_current (const int samples_number, const float sensibility_current, const uint8_t PIN_CURRENT_SENSOR, const int16_t CURRENT_OFFSET);

void force_balancing (const uint8_t TOTAL_IC, uint8_t tx_cfg[][6], uint8_t cells_to_balance, bool group);

void balancing(const uint8_t TOTAL_IC,  uint16_t cell_codes[][12], uint8_t tx_cfg[][6], float max_difference, uint8_t TOTAL_CELL, float VOV_THR, float VUV_THR );

float calc_volt_total(const uint16_t cell_codes[][12], const uint8_t TOTAL_IC, const uint8_t TOTAL_CELL) ;

float calculate_SOC(int current, float voltage, float resistor);

void calculate_SOH();

void start_cell_voltage_ADC(const uint8_t TOTAL_IC, uint8_t tx_cfg[][6]);

void serial_print_hex(uint8_t data);
 #endif
