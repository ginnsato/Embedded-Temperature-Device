/**
 * @file sleep_routines.c
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  04/02/2023
 *
 * @brief
 *  Header file for sleep routines
 *
 */

#ifndef SRC_HEADER_FILES_SLEEP_ROUTINES_H_
#define SRC_HEADER_FILES_SLEEP_ROUTINES_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_emu.h"
#include "em_core.h"
#include "em_assert.h"
//***********************************************************************************
// global variables
//***********************************************************************************

#define EM0               0u
#define EM1               1u
#define EM2               2u
#define EM3               3u
#define EM4               4u
#define MAX_ENERGY_MODES  5u

#define SOME_ERROR        -1


//***********************************************************************************
// function prototypes
//***********************************************************************************

void sleep_open(void);

void sleep_block_mode(uint32_t EM);

void sleep_unblock_mode(uint32_t EM);

void enter_sleep(void);

uint32_t current_block_energy_mode(void);

#endif /* SRC_HEADER_FILES_SLEEP_ROUTINES_H_ */
