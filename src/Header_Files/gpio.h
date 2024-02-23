/**
 * @file gpio.h
 *
 * @author
 *  Ginn Sato and Arielle Blum
 *
 * @date
 *  02/23/2023
 *
 * @brief
 *  Header file for gpio
 *
 */

#ifndef GPIO_HG
#define GPIO_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"
#include "brd_config.h"
#include "em_assert.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************

//#define GPIO_TEST_BIT_MASK        0x1000
#define GPIO_ODD_INT_PIN_7_MASK   0b010000000
#define GPIO_EVEN_INT_PIN_6_MASK  0b001000000

typedef struct {
  bool      enable;           // enable the GPIO upon completion of open
  uint32_t  btn0_pin;         // btn0 route to gpio pin
  uint32_t  btn0_port;        // btn0 route to gpio port
  uint32_t  btn1_pin;         // btn1 route to gpio pin
  uint32_t  btn1_port;        // btn1 route to gpio port
  bool      btn0_en;          // enable btn0 port/pin
  bool      btn1_en;          // enable btn1 port/pins
  bool      btn0_irq_enable;  // enable interrupt on btn0 interrupt
  bool      btn1_irq_enable;  // enable interrupt on btn0 interrupt
} GAME_GPIO_TypeDef;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_open(GAME_GPIO_TypeDef* game_gpio);

#endif
