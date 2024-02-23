/**
 * @file si7021.h
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  04/16/2023
 *
 * @brief
 *  Header file for si7021
 *
 */

#ifndef SRC_HEADER_FILES_SI7021_H_
#define SRC_HEADER_FILES_SI7021_H_


//***********************************************************************************
// Include files
//***********************************************************************************

#include "HW_delay.h"
#include "i2c.h"

//***********************************************************************************
// Defined files
//***********************************************************************************

#define Si7021_POWER_UP_DELAY   80u
#define Si7021_NUM_BYTES        2u
#define Si7021_CUR_BYTES        0u

//***********************************************************************************
// function prototypes
//***********************************************************************************

void si7021_open(void);

void si7021_read(uint32_t call_back);

uint32_t si7021_get_raw_data(void);

float si7021_calc_temp(uint32_t raw_data);


#endif /* SRC_HEADER_FILES_SI7021_H_ */
