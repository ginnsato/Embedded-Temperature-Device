/**
 * @file scheduler.h
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  03/07/2023
 *
 * @brief
 *  Header file for schedule
 *
 */

#ifndef SRC_HEADER_FILES_SCHEDULER_H_
#define SRC_HEADER_FILES_SCHEDULER_H_

#include <stdint.h>
#include "em_assert.h"
#include "em_core.h"
#include "em_emu.h"
#include "brd_config.h"
//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// global variables
//***********************************************************************************

#define NO_EVENTS 0

//***********************************************************************************
// function prototypes
//***********************************************************************************

void scheduler_open(void);

uint32_t get_scheduled_events(void);

void add_scheduled_event(uint32_t event);

void remove_scheduled_event(uint32_t event);

#endif /* SRC_HEADER_FILES_SCHEDULER_H_ */
