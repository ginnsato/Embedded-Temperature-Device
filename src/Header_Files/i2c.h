/**
 * @file i2c.h
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  04/11/2023
 *
 * @brief
 *  Header file for i2c
 *
 */


#ifndef SRC_HEADER_FILES_I2C_H_
#define SRC_HEADER_FILES_I2C_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_assert.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "scheduler.h"
#include "sleep_routines.h"

//***********************************************************************************
// Defined files
//***********************************************************************************

#define I2C_TEST_BIT 0b10
#define SCL_ROUTE_BIT_SHIFT 7u
#define SCL_EN_BIT_SHIFT 1u
#define INTERRUPT_DISABLE 0
#define I2C_ADDRESS_SHIFT 1u
#define I2C_EM_BLOCK      EM2

#define BUFFER_OFFSET      8u

#define Si7021_READ_TEMP_CMD    0x0F3U


//***********************************************************************************
// TypeDefs
//***********************************************************************************

typedef enum{
  I2C_INIT,
  I2C_RX_SEND_CMD,
  I2C_RX_SEND_ADDR,
  I2C_RX_RECEIVE_DATA,
  I2C_CLOSE
} tI2C_STATES;


typedef enum{
  WRITE,
  READ
} tI2C_CMD;

typedef struct{
  bool enable;
  bool master;
  uint32_t refFreq;
  uint32_t freq;
  I2C_ClockHLR_TypeDef clhr;
  uint32_t out_pin_route_SCL;     // out 0 route to gpio port/pin
  uint32_t out_pin_route_SDA;     // out 1 route to gpio port/pin
  bool out_pin_SCL_en;            // enable out 0 route
  bool out_pin_SDA_en;            // enable out 1 route
} I2C_Open_TypeDef;

typedef struct{
  tI2C_STATES current_state;
  I2C_TypeDef *i2c;
  volatile bool i2c_busy;
  uint32_t i2c_num_bytes;
  volatile uint32_t i2c_curr_bytes;
  uint32_t i2c_device_address;
  uint32_t i2c_register_address;
  uint32_t *i2c_data;
  tI2C_CMD i2c_cmd;
  uint32_t i2c_cb;
  uint32_t device_cmd;
} I2C_StateMachine_TypeDef;


//***********************************************************************************
// function prototypes
//***********************************************************************************

void i2c_open(I2C_TypeDef *i2c_peripheral, I2C_Open_TypeDef *i2c_open);

void i2c_start(I2C_StateMachine_TypeDef *i2c_sm);

void i2c_bus_reset(I2C_TypeDef *i2c_peripheral);



#endif /* SRC_HEADER_FILES_I2C_H_ */
