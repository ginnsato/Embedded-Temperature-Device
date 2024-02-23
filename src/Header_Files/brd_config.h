/**
 * @file brd_config.h
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  04/16/2023
 *
 * @brief
 *  Configure board
 *
 */



#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"
#include "em_cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// true and false defines
#define ENABLE            1
#define DISABLE           0


// LED 0 pin is
#define	LED0_PORT				  gpioPortF
#define LED0_PIN				  4u
#define LED0_DEFAULT			0 	// Default false (0) = off, true (1) = on
#define LED0_GPIOMODE			gpioModePushPull

// LED 1 pin is
#define LED1_PORT				  gpioPortF
#define LED1_PIN				  5u
#define LED1_DEFAULT			0	// Default false (0) = off, true (1) = on
#define LED1_GPIOMODE			gpioModePushPull

// Button 0
#define BUT0_PORT         gpioPortF
#define BUT0_PIN          6u
#define BUT0_DEFAULT      0
#define BUT0_GPIOMODE     gpioModeInput

// Button 1
#define BUT1_PORT         gpioPortF
#define BUT1_PIN          7u
#define BUT1_DEFAULT      0
#define BUT1_GPIOMODE     gpioModeInput

// Si7021

#define SI7021_I2C_ADDRESS      0x40

#define SI7021_SCL_PORT         gpioPortC
#define SI7021_SCL_PIN          11u
#define SI7021_SDA_PORT         gpioPortC
#define SI7021_SDA_PIN          10u
#define SI7021_SENSOR_EN_PORT   gpioPortB
#define SI7021_SENSOR_EN_PIN    10u

#define SI7021_SCL_MODE         gpioModeWiredAnd
#define SCL_DEFAULT             1u
#define SDA_DEFAULT             1u
#define SI7021_SDA_MODE         gpioModeWiredAnd

#define SI7021_SENSOR_EN_MODE   gpioModePushPull
#define SI7021_SENSOR_EN_OUT    1u

#define GPIO_EVEN_IRQ_CB      0b000000001
#define GPIO_ODD_IRQ_CB       0b000000010
#define APP_BTN0_CB           0b000000100
#define APP_BTN1_CB           0b000001000
#define APP_CHK_INPUT_CB      0b000010000
#define LETIMER_COMP0_IRQ_CB  0b000100000
#define LETIMER_COMP1_IRQ_CB  0b001000000
#define LETIMER_UF_IRQ_CB     0b010000000
#define SI7021_TEMP_READ_CB   0b100000000

#define MCU_HFXO_FREQ			cmuHFRCOFreq_26M0Hz

// GPIO pin setup
#define STRONG_DRIVE

#ifdef STRONG_DRIVE
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
  #define BUT0_DRIVE_STRENGTH   gpioDriveStrengthStrongAlternateStrong
  #define BUT1_DRIVE_STRENGTH   gpioDriveStrengthStrongAlternateStrong
#else
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
  #define BUT0_DRIVE_STRENGTH   gpioDriveStrengthWeakAlternateWeak
  #define BUT1_DRIVE_STRENGTH   gpioDriveStrengthWeakAlternateWeak
#endif


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
