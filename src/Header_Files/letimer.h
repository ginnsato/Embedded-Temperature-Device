/**
 * @file letimer.h
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  03/14/2023
 *
 * @brief
 *  header file for letimer
 *
 */

#ifndef SRC_HEADER_FILES_LETIMER_H_
#define SRC_HEADER_FILES_LETIMER_H_

//***********************************************************************************
// Include files
//***********************************************************************************

/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"
#include "em_letimer.h"
#include "em_gpio.h"
#include "brd_config.h"
/* The developer's include statements */
#include "scheduler.h"
#include "sleep_routines.h"

//***********************************************************************************
// global variables
//***********************************************************************************

#define LETIMER_HZ 1000u
#define REPEAT_COUNT 1u
#define LETIMER_EM EM4

#define ROUTE_1_BIT_SHIFT 8
#define ROUTE_1_EN_BIT_SHIFT 1
#define COMP_1_IRQ_BIT_SHIFT 1

#define UF_IRQ_BIT_SHIFT 2
#define LETIMER_CLEAR_IF 0x1F


// values for testing
#define LETIMER_TEST_BITMASK 1u



typedef struct {
  bool debugRun;                // True, keep LETIMER running while halted
  bool enable;                  // enable LETIMER upon completion of open()
  uint32_t out_pin_route0;      // out 0 route to gpio port/pin
  uint32_t out_pin_route1;      // out 1 route to gpio port/pin
  bool out_pin_0_en;            // enable out 0 route
  bool out_pin_1_en;            // enable out 1 route
  float period;                 // total period in seconds
  float active_period;          // part of period that is LLH in seconds
  bool comp0_irq_enable;        // enable interrupt on comp0 interrupt
  uint32_t comp0_cb;            // comp0 callback (unique for scheduler)
  bool comp1_irq_enable;        // enable interrupt on comp1 interrupt
  uint32_t comp1_cb;            // comp1 callback (unique for scheduler)
  bool uf_irq_enable;           // enable interrupt on underflow interrupt
  uint32_t uf_cb;               // underflow cb (unique for scheduler)
}APP_LETIMER_PWM_TypeDef;

//***********************************************************************************
// function prototypes
//***********************************************************************************

void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct);
void letimer_start(LETIMER_TypeDef *letimer, bool enable);

#endif /* SRC_HEADER_FILES_LETIMER_H_ */
