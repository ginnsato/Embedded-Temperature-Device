/**
 * @file app.h
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  02/23/2023
 *
 * @brief
 *  Header file for app
 *
 */


#ifndef APP_HG
#define APP_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "si7021.h"
//***********************************************************************************
// global variables
//***********************************************************************************

//#define APP_BTN0_CB 0b0001
//#define APP_BTN1_CB 0b0010
#define PRESS       1
#define NONE        0
#define TWICE       2

#define AMBIENT_TEMP  26u       // temperature in degrees C

/*
// Application scheduled events (Bits 0-4 are covered in the brd_config for the state machine)
#define LETIMER0_COMP0_CB 0b00100000
#define LETIMER0_COMP1_CB 0b01000000
#define LETIMER0_UF_CB    0b10000000
*/

#define PWM_PER 3.0          // PWM period (s)
#define PWM_ACT_PER 0.025    // PWM active period (s)


// TODO
#define OUT0_ROUTE _LETIMER_ROUTELOC0_OUT0LOC_LOC29
#define OUT1_ROUTE _LETIMER_ROUTELOC0_OUT1LOC_LOC27


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);

void scheduled_gpio_even_irq_cb(void);

void scheduled_gpio_odd_irq_cb(void);

void app_state_machine(void);

void scheduled_record_button_press_cb(uint32_t button);

void scheduled_letimer_comp0_cb(void);

void scheduled_letimer_comp1_cb(void);

void scheduled_letimer_uf_cb(void);

void scheduled_read_i2c_cb(void);

#endif
