/**
 * @file HW_delay.h
 *
 * @author
 *  kgraham
 *
 * @date
 *  03/16/2019
 *
 * @brief
 *  Header file for HW_delay
 *
 */

#ifndef SRC_HW_DELAY_H_
#define SRC_HW_DELAY_H_

#include "em_timer.h"
#include "em_cmu.h"

void timer_delay(uint32_t ms_delay);

#endif /* SRC_HW_DELAY_H_ */
