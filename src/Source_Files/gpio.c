/**
 * @file gpio.c
 *
 * @author
 *  Ginn Sato and Arielle Blum
 *
 * @date
 *  02/23/2023
 *
 * @brief
 *  Enabling the desired GPIO settings, including enabling interrupts if requested
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"


//***********************************************************************************
// private variables
//***********************************************************************************
static uint32_t gpio_even_irq_cb = GPIO_EVEN_IRQ_CB;
static uint32_t gpio_odd_irq_cb = GPIO_ODD_IRQ_CB;

//***********************************************************************************
// function prototypes
//***********************************************************************************

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
* @brief
*  Enable the interrupts for GPIO peripheral
*
* @details
*  Enables the interrupts for BTN0 and BTN1 based on input parameter
*
*  @param [in] GAME_GPIO_TypeDef
*  Take in the struct of the game gpio configuration and set the corresponding
*  interrupts to enabled based on values of struct
*
******************************************************************************/
void gpio_start(GAME_GPIO_TypeDef *game_gpio){

  // configure button interrupts
    if(game_gpio->btn0_irq_enable){
        GPIO->IFC = GPIO_EVEN_INT_PIN_6_MASK;       // clear flag on interrupt pin 6
        GPIO_ExtIntConfig(game_gpio->btn0_port, game_gpio->btn0_pin, game_gpio->btn0_pin, false, true, game_gpio->btn0_irq_enable);
    }
    if(game_gpio->btn1_irq_enable){
        GPIO->IFC = GPIO_ODD_INT_PIN_7_MASK;       // clear flag on interrupt pin 7
        GPIO_ExtIntConfig(game_gpio->btn1_port, game_gpio->btn1_pin, game_gpio->btn1_pin, false, true, game_gpio->btn1_irq_enable);
    }

}


/***************************************************************************//**
 * @brief
 *  Enable the necessary things to get GPIO working
 *
 * @details
 *  Enable the CMU clock as well as set the strength and pin mode for the
 *  LEDs and buttons. Call function to enable interrupts.
 *
 *  @param [in] GAME_GPIO_TypeDef
 *  Take in the struct of the game gpio configuration and set the corresponding
 *  buttons to enabled based on values of struct
 *
 ******************************************************************************/
void gpio_open(GAME_GPIO_TypeDef* game_gpio){

  // Peripheral clock enabled
  CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, SI7021_SENSOR_EN_MODE, SI7021_SENSOR_EN_OUT);

	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, SI7021_SCL_MODE, SCL_DEFAULT);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, SI7021_SDA_MODE, SDA_DEFAULT);

	// Configure Button pins
  if(game_gpio->btn0_en){
      GPIO_DriveStrengthSet(BUT0_PORT, BUT0_DRIVE_STRENGTH);
      GPIO_PinModeSet(BUT0_PORT, BUT0_PIN, BUT0_GPIOMODE, BUT0_DEFAULT);
  }
  if(game_gpio->btn1_en){
      GPIO_DriveStrengthSet(BUT1_PORT, BUT1_DRIVE_STRENGTH);
      GPIO_PinModeSet(BUT1_PORT, BUT1_PIN, BUT1_GPIOMODE, BUT1_DEFAULT);
  }

  gpio_start(game_gpio);
  game_gpio->enable = true;
}


/***************************************************************************//**
 * @brief
 *  GPIO Interrupt Handler for Even Interrupts
 *
 * @details
 *  Check what interrupt was called based on the flag and the interrupts enabled.
 *  Make sure that this corresponds to the pin 6 interrupt, or button 0.
 *  Clear the corresponding interrupt's flag and assert that it was properly
 *  cleared.
 *
 *
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void){
  uint32_t flag = (((GPIO->IF) & (GPIO->IEN)) & GPIO_EVEN_INT_PIN_6_MASK);    // check pin 6 interrupt
  GPIO->IFC = flag;                                                           // clear the flag
  EFM_ASSERT(!(GPIO->IF & GPIO_EVEN_INT_PIN_6_MASK));                         // assert flag was properly cleared
  add_scheduled_event(gpio_even_irq_cb);                                      // add the event to scheduler
}



/***************************************************************************//**
 * @brief
 *  GPIO Interrupt Handler for Odd Interrupts
 *
 * @details
 *  Check what interrupt was called based on the flag and the interrupts enabled.
 *  Make sure that this corresponds to the pin 7 interrupt, or button 1.
 *  Clear the corresponding interrupt's flag and assert that it was properly
 *  cleared.
 *
 *
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void){
  uint32_t flag = (((GPIO->IF) & (GPIO->IEN)) & GPIO_ODD_INT_PIN_7_MASK);     // check pin 7 interrupt
  GPIO->IFC = flag;                                                           // clear the flag
  EFM_ASSERT(!(GPIO->IF & GPIO_ODD_INT_PIN_7_MASK));                          // assert flag was properly cleared
  add_scheduled_event(gpio_odd_irq_cb);                                      // add the event to scheduler
}
